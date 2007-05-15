<?php
// $Id$

function schema_pgsql_engine_type_map() {
  static $map;
  if (!isset($map)) {
    // put :normal last so it gets preserved by array_flip
    $map = array(
      'varchar:normal' => 'varchar',
      
      'text:small' => 'text',
      'text:medium' => 'text',
      'text:big' => 'text',
      'text:normal' => 'text',
      
      'int:tiny' => 'smallint',
      'int:small' => 'smallint',
      'int:medium' => 'integer',
      'int:big' => 'bigint',
      'int:normal' => 'integer',
      
      'float:tiny' => 'real',
      'float:small' => 'real',
      'float:medium' => 'real',
      'float:big' => 'double precision',
      'float:normal' => 'real',
      
      'blob:big' => 'bytea',
      'blob:normal' => 'bytea',

      'datetime:normal' => 'timestamp',

      'serial:normal' => 'serial',
      'serial:big' => 'bigserial',
      );
  }
  return $map;
}

function schema_pgsql_schema_type_map() {
  static $map;
  if (!isset($map)) {
    $map = array_flip(schema_pgsql_engine_type_map());
    $map['character varying'] = 'varchar:normal';
  }
  return $map;
}

function schema_pgsql_create_table_sql($table) {
  $sql_cols = array();
  foreach ($table['cols'] as $colname => $col) {
    $sql_cols[] = schema_pgsql_create_column_sql($colname, $col);
  }

  $sql_keys = array();
  if (isset($table['primary']) && is_array($table['primary'])) {
    $sql_keys[] = 'PRIMARY KEY ('.implode(', ', $table['primary']).')';
  }
  if (isset($table['unique keys']) && is_array($table['unique keys'])) {
    foreach ($table['unique keys'] as $keyname => $key) {
      $sql_keys[] = 'UNIQUE ('.implode(', ', $key).')';
    }
  }

  $sql = "CREATE TABLE {".$table['name']."} (\n\t";
  $sql .= implode(",\n\t", $sql_cols);
  if (count($sql_keys) > 0) {
    $sql .= ",\n\t";
  }
  $sql .= implode(",\n\t", $sql_keys);
  $sql .= "\n)";
  $stmts[] = $sql;

  if (is_array($table['indexes'])) {
    foreach ($table['indexes'] as $keyname => $key) {
      $stmts[] = ('CREATE INDEX {'.$table['name'].'}_'.
        implode('_', $key).'_idx ON {'.$table['name'].'} ('.
        implode(', ', $key).')');
    }
  }

  return $stmts;
}

function schema_pgsql_create_column_sql($colname, $col) {
  $sql = $colname.' '.schema_engine_type($col, 'pgsql');
  if ($col['type'] == 'serial') {
    unset($col['not null']);
  }
  unset($col['type']);
  if ($col['length']) {
    $sql .= '('.$col['length'].')';
  }
  unset($col['length']);
  if ($col['not null']) {
    $sql .= ' NOT NULL';
  }
  unset($col['not null']);
  foreach ($col as $prop => $val) {
    switch ($prop) {
      case 'default':
        $sql .= " $prop ";
        if (is_string($val)) {
          $sql .= "'$val'";
        } else {
          $sql .= $val;
        }
        break;
    }
  }
  return $sql;
}

function schema_pgsql_inspect($tbl_name = NULL) {
  global $db_url;

  $tables = array();
  $url = parse_url($db_url);
  $database = substr($url['path'], 1);

  //
  // Retrieve information about all columns in the database from the
  // 'columns' table of 'information_schema'.  In pgsql, TABLE_CATALOG
  // is the database name and we provide $database to db_query below.
  // We sort the columns by table_name and ordinal_position so the get
  // added to our array in the same order.
  //
  $sql = ('SELECT * FROM information_schema.COLUMNS '.
	  'WHERE table_catalog=\'%s\' AND table_schema=\'public\'');
  if (isset($tbl_name)) {
    $sql .= 'AND table_name = \'%s\' ';
  }
  $sql .= 'ORDER BY table_name, ordinal_position';
  
  $res = db_query($sql, $database, $tbl_name);

  //
  // Add an entry to $tables[<tablename>]['fields'] for each column.  $r
  // is a row from information_schema.columns.  $col is the Schema
  // column structure we build up from it.
  //
  while ($r = db_fetch_array($res)) {
    $col = array();

    // We treat numeric columns slightly differently and identify them
    // because they have a 'numeric_scale' property.
    $numeric = !is_null($r['numeric_scale']);

    // Determine the Schema type and size from the database data_type.
    list($col['type'], $col['size']) = schema_schema_type($r['data_type'],
      'pgsql');

    // Non-numeric columns (e.g. varchar) can have a 'length'.
    if (! $numeric && $r['character_maximum_length']) {
      $col['length'] = $r['character_maximum_length'];
    }

    // Any column can have NOT NULL.
    $col['not null'] = ($r['is_nullable'] == 'YES' ? FALSE : TRUE);

    // Any column might have a default value.  We have to set
    // $col['default'] to the correct type of data.  Remember that '',
    // 0, and '0' are all different types.
    if (! is_null($r['column_default'])) {

      // pgsql's column_default can have ::typename appended,
      // nextval('<sequence_name>') if it is serial, etc.  Here, we're
      // just splitting out the actual default value.
      if (strpos($r['column_default'], '::') !== FALSE) {
        list($col['default'], $def_type) = explode('::', $r['column_default']);
      } else {
        $col['default'] = $r['column_default'];
        $def_type = '';
      }
      
      if ($numeric) {
        // $col['default'] is currently a string.  If the column is
        // numeric, use intval() or floatval() to extract the value as a
        // numeric type.

        // more pgsql-specific stuff
        if (strpos($col['default'], 'nextval(\'') !== FALSE &&
          $def_type == 'regclass)') {
          $col['type'] = 'serial';
          unset($col['default']);
        } 
        else if ($col['type'] == 'float') {
          $col['default'] = floatval($col['default']);
        } else {
          $col['default'] = intval($col['default']);
        }
      } else {
        // The column is not numeric, so $col['default'] should remain
        // a string.  However, pgsql returns $r['column_default']
        // wrapped in single-quotes.  We just want the string value,
        // so strip off the quotes.
        $col['default'] = substr($col['default'], 1, -1);
      }

    }

    // Set $col['unsigned'] if the column is unsigned.
    // XXX These domains are currently defined in system.install.
    // This engine should probably create and own them, however.
    switch ($r['domain_name']) {
      case 'int_unsigned':
      case 'smallint_unsigned':
      case 'bigint_unsigned':
        $col['unsigned'] = 1;
        break;
    }
    if (isset($r['check_clause']) && $r['check_clause'] == '(('. $r['column_name'] .' => 0))') {
      $col['unsigned'] = 1;
    }

    if (FALSE && !isset($once) && $r['column_name'] == 'bid' && $r['table_name'] == 'boxes') {
      dpm($r);
      dpm($col);
      $once = 1;
    }

    // Save the column definition we just derived from $r.
    $tables[$r['table_name']]['fields'][$r['column_name']] = $col;
    $tables[$r['table_name']]['name'] = $r['table_name'];
  }

  //
  // Make sur we caught all the unsigned columns.  I could not get
  // this to work as a left join on the previous query.
  //
  $res = db_query('SELECT ccu.*, cc.check_clause '.
    'FROM information_schema.constraint_column_usage ccu '.
    'INNER JOIN information_schema.check_constraints cc '.
    '  ON ccu.constraint_name=cc.constraint_name '.
    'WHERE table_schema=\'public\'');
  while ($r = db_fetch_array($res)) {
    if ($r['check_clause'] == '(('. $r['column_name'] .' >= 0))') {
      $tables[$r['table_name']]['fields'][$r['column_name']]['unsigned'] =TRUE;
    }
  }

  //
  // Retrieve information about all keys in the database from the
  // 'key_column_usage' table of 'information_schema'.  Again, we sort
  // the columns by table_name, constraint_name, and ordinal_position
  // so the get added to our array in the same order; in the case of
  // keys, the order really matters.
  //
  $res = db_query('SELECT * FROM information_schema.key_column_usage kcu '.
    'INNER JOIN information_schema.table_constraints tc '.
    '  ON tc.constraint_name=kcu.constraint_name '.
    'WHERE kcu.table_catalog=\'%s\' AND kcu.table_schema=\'public\' '.
    'ORDER BY kcu.table_name, kcu.constraint_name, kcu.ordinal_position',
    $database);

  //
  // Add an entry to $tables[<tablename>]['primary key'] or ['unique
  // keys'] for each column involed in a key.  $r is a row from
  // information_schema.  Remember that 'primary key' is an array of
  // column names but 'unique keys' is a hash table mapping key name
  // to an array of column names.
  //
  // Normalize key names <table>_<cols>_key to just <cols>.
  //
  while ($r = db_fetch_array($res)) {
    $name = $r['constraint_name'];
    if (preg_match('@^'.$r['table_name'].'_(.*)_key$@', $name, $m)) {
      $name = $m[1];
    }
    if ($r['constraint_type'] == 'PRIMARY KEY') {
      $tables[$r['table_name']]['primary key'][] = $r['column_name'];
    } else {
      $tables[$r['table_name']]['unique keys'][$name][] = $r['column_name'];
    }
  }

  //
  // Retrieve information about all indices (non-unique keys) in the
  // database.  pgsql does not represent indices in information_schema
  // so we have to use the pgsql-specific pg_* tables for this.
  // Again, sort order matters.
  //
  // I truly wish I was kidding with this ridiculous query, but I'm
  // not.
  // 
  // Ruby on Rails (unsurprisingly) needs similar code, and I found
  // this monstrous query at http://dev.rubyonrails.org/ticket/6253.
  // Mine isn't exactly the same as their's, so quite possibly mine is
  // wrong.  Also note http://dev.rubyonrails.org/ticket/5819 which
  // uses the ANY keyword instead of the OR list to make a simpler
  // query but it requires PG 8.1.
  //
  $res = db_query(
    'SELECT t.relname as tblname, a.attname, c.relname as idxname, i.indexrelid, i.indkey FROM pg_index i '.
    'INNER JOIN pg_class c ON i.indexrelid = c.oid '.
    'INNER JOIN pg_class t ON i.indrelid = t.oid '.
    'INNER JOIN pg_namespace n ON t.relnamespace = n.oid '.
    'INNER JOIN pg_attribute a ON a.attrelid = t.oid AND ('.
    '   a.attnum = i.indkey[0] OR '.
    '   a.attnum = i.indkey[1] OR '.
    '   a.attnum = i.indkey[2] OR '.
    '   a.attnum = i.indkey[3] OR '.
    '   a.attnum = i.indkey[4] OR '.
    '   a.attnum = i.indkey[5] OR '.
    '   a.attnum = i.indkey[6] OR '.
    '   a.attnum = i.indkey[7] OR '.
    '   a.attnum = i.indkey[8] OR '.
    '   a.attnum = i.indkey[9]) '.
    'WHERE i.indisunique != \'t\' AND i.indisprimary != \'t\' AND '.
    '      n.nspname=\'public\' '.
    'ORDER BY idxname, CASE '.
    '        WHEN i.indkey[0]=a.attnum THEN 0 '.
    '        WHEN i.indkey[1]=a.attnum THEN 1 '.
    '        WHEN i.indkey[2]=a.attnum THEN 2 '.
    '        WHEN i.indkey[3]=a.attnum THEN 3 '.
    '        WHEN i.indkey[4]=a.attnum THEN 4 '.
    '        WHEN i.indkey[5]=a.attnum THEN 5 '.
    '        WHEN i.indkey[6]=a.attnum THEN 6  '.
    '        WHEN i.indkey[7]=a.attnum THEN 7 '.
    '        WHEN i.indkey[8]=a.attnum THEN 8 '.
    '        WHEN i.indkey[9]=a.attnum THEN 9 END ');

  //
  // Add an entry to $tables[<tablename>]['indexes'] for each column
  // involed in an index (non-unique key).  Remember that ['indexes'] is
  // like ['unique keys']: a hash table mapping index name to an array of
  // column names.
  //
  while ($r = db_fetch_array($res)) {
    $name = $r['idxname'];
    if (preg_match('@^'.$r['tblname'].'_(.*)_idx$@', $name, $m)) {
      $name = $m[1];
    }
    $tables[$r['tblname']]['indexes'][$name][] = $r['attname'];
  }

  // All done!  Visit admin/build/schema/inspect to see the
  // pretty-printed version of what gets returned here and verify if
  // it is correct.
  return $tables;
}

?>