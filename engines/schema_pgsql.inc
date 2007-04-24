<?php
// $Id$

// Table of MySQL, PostgreSQL, and ANSI SQL data types:
// http://en.wikibooks.org/w/index.php?title=Programming:Converting_MySQL_to_PostgreSQL#Data_Types

function _schema_pgsql_type_map() {
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
      );
  }
  return $map;
}

function _schema_type_to_pg($col) {
  $map = _schema_pgsql_type_map();
  $size = (isset($col['size']) ? $col['size'] : 'normal');
  $type = $col['type'].':'.$size;
  if (isset($map[$type])) {
    return $map[$type];
  } else {
    drupal_set_message(t('No PostgreSQL type for Schema type %type.',
                         array('%type' => $type)), 'error');
    return $col['type'];
  }
}

function _schema_pg_to_type($type) {
  $map = array_flip(_schema_pgsql_type_map()); // inefficient
  $map['character varying'] = 'varchar:normal'; // XXX
  if (isset($map[$type])) {
    return explode(':', $map[$type]);
  } else {
    drupal_set_message(t('No Schema type for PostgreSQL type %type.',
                         array('%type' => $type)), 'error');
    return array($type, 'normal');
  }
}

function schema_pgsql_create_table_sql($table) {
  $sql_cols = array();
  foreach ($table['cols'] as $colname => $col) {
    $sql = $colname.' '._schema_type_to_pg($col);
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
    $sql_cols[] = $sql;
  }

  $sql_keys = array();
  if (is_array($table['keys'])) { // tables indexes are not required
    foreach ($table['keys'] as $keyname => $key) {
      $sql = '';
      if ($keyname == 'PRIMARY') {
        $sql = 'PRIMARY KEY ';
      } else {
        if ($key[1] == 'UNIQUE') {
          $sql = 'UNIQUE ';
        }
        $sql .= 'KEY '.$keyname.' ';
      }
      $sql .= '('.$key[0].')';
      $sql_keys[] = $sql;
    }
  }

  $sql = "CREATE TABLE {".$table['name']."} (\n\t";
  $sql .= implode(",\n\t", $sql_cols);
  if (count($sql_keys) > 0) {
    $sql .= ",\n\t";
  }
  $sql .= implode(",\n\t", $sql_keys);
  $sql .= "\n";
  $sql .= ");\n\n";
  return $sql;
}

function schema_pgsql_inspect($name = NULL) {
  global $db_url;

  $tables = array();
  $url = parse_url($db_url);
  $database = substr($url['path'], 1);

  $sql = ('SELECT * FROM information_schema.COLUMNS '.
	  'WHERE TABLE_CATALOG=\'%s\' AND TABLE_SCHEMA=\'public\'');
  if (isset($name)) {
    $sql .= 'AND TABLE_NAME = \'%s\' ';
  }
  $sql .= 'ORDER BY TABLE_NAME, ORDINAL_POSITION';
  
  $res = db_query($sql, $database, $name);
  while ($r = db_fetch_array($res)) {
    if (!$once && $r['data_type'] == 'character varying') {
      //dpm($r);
      $once++;
    }

    $numeric = !is_null($r['numeric_scale']);
    $col = array();
    list($col['type'], $col['size']) = _schema_pg_to_type($r['data_type']);
    if (! $numeric) {
      $col['length'] = $r['character_maximum_length'];
    }
    $col['not null'] = ($r['is_nullable'] == 'YES' ? 0 : 1);
    if (! is_null($r['column_default'])) {
      if ($numeric) {
        // XXX floats!
        $col['default'] = intval($r['column_default']);
      } else {
        $col['default'] = $r['column_default'];
        // XXX I don't really know what ::character varying, ::text,
        // etc. is for.
        $col['default'] = preg_replace('@::.*@', '', 
          $col['default']);
      }
    }
    $tables[$r['table_name']]['cols'][$r['column_name']] = $col;
    $tables[$r['table_name']]['name'] = $r['table_name'];
  }

  $res = db_query('SELECT * FROM information_schema.key_column_usage kcu '.
    'INNER JOIN information_schema.table_constraints tc '.
    '  ON tc.constraint_name=kcu.constraint_name '.
    'WHERE kcu.table_catalog=\'%s\' AND kcu.table_schema=\'public\' '.
    'ORDER BY kcu.table_name, kcu.constraint_name, kcu.ordinal_position',
    $database);
  while ($r = db_fetch_array($res)) {
    $name = $r['constraint_name'];
    if ($r['constraint_type'] == 'PRIMARY KEY') {
      $name = 'PRIMARY';
    }
    if (isset($tables[$r['table_name']]['keys'][$name][0])) {
      $tables[$r['table_name']]['keys'][$name][0] .= ', ';
    }
    $tables[$r['table_name']]['keys'][$name][0] .=$r['column_name'];
    // primary key is always unique
    if ($name != 'PRIMARY') {
      // in fact, all pgsql constraints are unique
      $tables[$r['table_name']]['keys'][$name][1] = 'unique';
    }
  }

  // I truly wish I was kidding here, but I'm not.
  // 
  // RoR (unsurprisingly) needs similar code, and I found this
  // monstrous query at http://dev.rubyonrails.org/ticket/6253.  Mine
  // isn't exactly the same as their's, so quite possibly mine is
  // wrong.
  //
  // Also: http://dev.rubyonrails.org/ticket/5819, uses the ANY
  // keyword instead of the OR list but requires PG 8.1.
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
  while ($r = db_fetch_array($res)) {
    //dpm($r);
    $tables[$r['tblname']]['keys'][$r['idxname']][0] .= $r['attname'].', ';
    $tables[$r['tblname']]['keys'][$r['idxname']][1] .= 'multi';
  }

  return $tables;
}

?>