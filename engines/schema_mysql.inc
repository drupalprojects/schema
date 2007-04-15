<?php
// $Id$

function schema_mysql_create_table_sql($table) {
  $sql_cols = array();
  foreach ($table['cols'] as $colname => $col) {
    $sql = $colname.' '.$col['type'];
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
  $sql .= ") /*!40100 DEFAULT CHARACTER SET utf8 */;\n\n";
  return $sql;
}

function schema_mysql_inspect($name = NULL) {
  global $db_url;

  $tables = array();
  $url = parse_url($db_url);
  $database = substr($url['path'], 1);

  $sql = ('SELECT * FROM information_schema.COLUMNS '.
	  'WHERE TABLE_SCHEMA="%s" ');
  if (isset($name)) {
    $sql .= 'AND TABLE_NAME = "%s" ';
  }
  $sql .= 'ORDER BY TABLE_NAME, ORDINAL_POSITION';

  $res = db_query($sql, $database, $name);
  while ($r = db_fetch_array($res)) {
    $numeric = !is_null($r['NUMERIC_SCALE']);
    $col = array();
    $col['type'] = $r['COLUMN_TYPE'];
    if (preg_match('@([a-z]+)(?:\((\d+)\))?\s*(unsigned)?@', $col['type'],
          $matches)) {
      $col['type'] = $matches[1];
      if ($matches[2]) {
        if ($numeric) {
          $col['disp_width'] = $matches[2];
        } else {
          $col['length'] = $matches[2];
        }
      }
      if ($matches[3]) {
        $col['unsigned'] = TRUE;
      }
    }
    $col['not null'] = ($r['IS_NULLABLE'] == 'YES' ? 0 : 1);
    if (! is_null($r['COLUMN_DEFAULT'])) {
      if ($numeric) {
        // XXX floats!
        $col['default'] = intval($r['COLUMN_DEFAULT']);
      } else {
        $col['default'] = $r['COLUMN_DEFAULT'];
      }
    }
    $tables[$r['TABLE_NAME']]['cols'][$r['COLUMN_NAME']] = $col;
    $tables[$r['TABLE_NAME']]['name'] = $r['TABLE_NAME'];
  }

  $res = db_query('SELECT * FROM information_schema.STATISTICS '.
    'WHERE TABLE_SCHEMA="%s" '.
    'ORDER BY TABLE_NAME, INDEX_NAME, SEQ_IN_INDEX',
    $database);
  while ($r = db_fetch_array($res)) {
    if (isset($tables[$r['TABLE_NAME']]['keys'][$r['INDEX_NAME']][0])) {
      $tables[$r['TABLE_NAME']]['keys'][$r['INDEX_NAME']][0] .= ', ';
    }
    $tables[$r['TABLE_NAME']]['keys'][$r['INDEX_NAME']][0] .=$r['COLUMN_NAME'];
    // primary key is always unique
    if ($r['INDEX_NAME'] != 'PRIMARY') {
      $tables[$r['TABLE_NAME']]['keys'][$r['INDEX_NAME']][1] = 
      ($r['NON_UNIQUE'] == 0 ? 'unique' : 'multi');
    }
  }

  return $tables;
}

?>