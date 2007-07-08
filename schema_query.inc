<?php
//////////////////////////////////////////////////////////////////////
// Schema query stuff.  EXPERIMENTAL, do not try to use this yet.
//////////////////////////////////////////////////////////////////////

function x_schema_schema_alter(&$schema) {
  // Compute and store inverted join information so each table knows
  // which other tables join to it.
  foreach ($schema as $t_name => $table) {
      $module = $table['module'];
      foreach ($table['fields'] as $c_name => $col) {
        if (isset($col['join'])) {
          list($to_mod, $to_tab, $to_col) = $col['join'];
          $schema[$to_mod][$to_tab]['join-from'][] = array(
            $to_col, $module, $t_name, $c_name, $col['join-type']);
        }
      }
  }

  // Generate the SQL query for each table.
  foreach ($schema as $t_name => $table) {
      _schema_generate_table($schema, $t_name);
  }

  if (module_exists('node')) {
    $node_types = node_get_types();
    foreach ($node_types as $type) {
      _schema_generate_table($schema, 'node', $type->type);
    }
  }
}

// Generate the SELECT query and associated data to retrieve a
// complete entity from $module's table $t_name.
//
// If $o_type is provided (e.g. "story" if $module is "node")
// eliminate joins to tables that specify which $module types they
// join to and do not include $o_type in the list.  This eliminates
// joins that will never produce any results.  It is currently only
// used for node.
function _schema_generate_table(&$schema, $t_name, $o_type = NULL) {
  $table = $schema[$t_name];

  // We can't build a WHERE clause for a table without a primary key.
  if (!isset($table['primary key'])) {
    return;
  }
  // SELECT each column individually from the primary table so
  // we can access it by name.
  foreach ($table['fields'] as $c_name => $col) {
    $sel_as = "${t_name}_${c_name}";
    $selcols[$t_name][] = $sel_as;
    $sels[] = "$t_name.$c_name as $sel_as";
  }
  foreach ($table['primary key'] as $pcol) {
    $primarys[$t_name][] = "${t_name}_${pcol}";
    switch ($table['fields'][$pcol]['type']) {
      case 'int': 
      case 'float':
        $wheres[] = "${t_name}.${pcol}=%d";
        break;
      default:
        $wheres[] = "${t_name}.{$pcol}='%s'";
        break;
    }
  }

  $joins = array();
  if (!isset($schema[$t_name]['join-from'])) {
    $schema[$t_name]['join-from'] = array();
  }

  $join_many = 0;
  foreach ($schema[$t_name]['join-from'] as $join) {
    list($my_col, $to_mod, $to_tab, $to_col, $join_type) = $join;
    $table = $schema[$to_mod][$to_tab];

    if (isset($o_type) && is_array($table['fields'][$to_col][$t_name.'-types'])&&
      array_search($o_type, $table['fields'][$to_col][$t_name.'-types'])
      === FALSE) {
      continue;
    }
    
    // SELECT each column individually from the joined table so
    // we can access it by name.
    foreach ($table['fields'] as $c_name => $col) {
      $sel_as = "${to_tab}_${c_name}";
      $selcols[$to_tab][] = $sel_as;
      $sels[] = "$to_tab.$c_name as $sel_as";
    }

    if (isset($table['primary key'])) {
      foreach ($table['primary key'] as $pcol) {
        $primarys[$to_tab][] = "${to_tab}_${pcol}";
      }
    } else {
      drupal_set_message("Schema: join to $t_name requires a primary key", 'error');
      break;
    }

    // LEFT JOIN to each table.  We need to use LEFT JOIN
    // because only a subset of the tables will have any data
    // for this node.
    $joins[] ="LEFT JOIN {$to_tab} $to_tab ON $t_name.$my_col=$to_tab.$to_col";
    $jointypes[$to_tab] = $join_type;
    if ($join_type == 'many') {
      $join_many++;
    }
  }

  // Execute our rather large assembled query.  Not just for Views anymore!
  $sql = ('SELECT '.implode(",\n\t", $sels)."\nFROM {$t_name} $t_name\n".
	  implode("\n\t", $joins)."\nWHERE ".
	  implode(' AND ', $wheres));
  //dpm($sql);

  if (isset($o_type)) {
    $schema[$t_name]['query-'.$o_type] = $sql;
    $schema[$t_name]['selcols-'.$o_type] = $selcols;
    $schema[$t_name]['primarys-'.$o_type] = $primarys;
  } else {
    $schema[$t_name]['query'] = $sql;
    $schema[$t_name]['selcols'] = $selcols;
    $schema[$t_name]['primarys'] = $primarys;
  }
  if (isset($jointypes)) {
    $schema[$t_name]['jointypes'] = $jointypes;
  }
}

function schema_nodeapi(&$node, $op, $a3, $a4) {
  return;

  switch ($op) {
    case 'load':
      if (! isset($node->schema_data)) {
        return schema_node_load($node->nid, $node->type, $node);
      }
      break;

    case 'view':
      // For easy debugging and demonstration, display the data we
      // loaded in the node body.  This is temporary, of course.
      $node->content['schema_data'] = array(
        '#title' => 'schema_data',
        '#value' => '<p><b>data loaded by schema in one query</b></p><textarea style="width:100%" rows="30">'.print_r($node->schema_data,1).'</textarea>',
        '#weight' => 99);
      
      break;
  }
}

function schema_node_load($nid, $ntype = NULL, $node = NULL) {
  $schema = schema_invoke_all();

  if (isset($ntype) && 
    isset($schema['system']['node']['query-'.$ntype])) {
    $sql = $schema['system']['node']['query-'.$ntype];
    $selcols = $schema['system']['node']['selcols-'.$ntype];
    $primarys = $schema['system']['node']['primarys-'.$ntype];
  } else {
    $sql = $schema['system']['node']['query'];
    $selcols = $schema['system']['node']['selcols'];
    $primarys = $schema['system']['node']['primarys'];
  }
  $jointypes = $schema['system']['node']['jointypes'];

  //dpm($sql);
  $q = db_query($sql, $nid);

  // Each row of the results will have values from one or more
  // tables.  We saved the list of columns selected from each
  // table above.  For each row, if any of the columns from a
  // particular table are non-NULL, save all of that table's
  // columns for this row.  If all of the columns are NULL, we are
  // seeing LEFT JOIN artifact, so we skip it.
  //
  // The nature of this query is that we get a lot of duplicated
  // data and we have to be sure to ignore all but one copy of
  // each unique result.  For one-joins, it is easy: after we get
  // results for that table we just stop looking for them.  For
  // many-joins, we keep track of the primary keys we've seen from
  // that table and only save a row's data if we haven't seen that
  // primary key yet (by definition, primary keys are unique).
  //
  // Save all the data in $node->schema_data, an array mapping
  // table names to an array of result rows.
  while ($r = db_fetch_array($q)) {
    foreach ($selcols as $table => $cols) {
      $vals = array();
      $got_val = FALSE;
      foreach ($cols as $sel_as) {
        $vals[$sel_as] = $r[$sel_as];
        $got_val = $got_val || !is_null($r[$sel_as]);
      }
      if ($got_val) {
        $keys = $primarys[$table];
        $keyvals = implode('::', array_intersect_key($vals, array_flip($primarys[$table])));
        if (isset($schema_data[$table][$keyvals])) {
          continue;
        }
        $tlen = strlen($table)+1;
        foreach ($vals as $key=>$val) {
          $schema_data[$table][$keyvals][substr($key, $tlen)] = $val;
        }
            
        if ($jointypes[$table] == 'one') {
          // ignore any repeated data from this table
          unset($selcols[$table]);
        }
      }
    }
  }

  // If we're being called by node_load, assemble the node object it
  // expects.  XXX This is a kludge.  users and node both define column
  // 'status', but we want node's, so we list userse first.  This is
  // just a temporary hack anyway; if node_load() wants to use Schema
  // it should just use schema_data directly or assemble the fields of
  // the $node object itself.
  if (is_null($node)) {
    $node = array_merge(
      $schema_data['users'][$schema_data['node'][$nid]['uid']],
      $schema_data['node_revisions'][$schema_data['node'][$nid]['vid']],
      $schema_data['node'][$nid]
      );
    $node = (object) $node;
  }

  $node->schema_data = $schema_data;
  return $node;
}

?>