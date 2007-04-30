<?php
// $Id$

function schema_mysql_engine_type_map() {
  static $map;
  if (!isset($map)) {
    $map = array(
      'varchar:normal' => 'varchar',
      
      'text:small' => 'smalltext',
      'text:medium' => 'mediumtext',
      'text:big' =>  'longtext',
      'text:normal' => 'text',

      'serial:normal' => 'int',
      'serial:big' => 'bigint',

      'int:tiny' => 'tinyint',
      'int:small' => 'smallint',
      'int:medium' => 'mediumint',
      'int:big' => 'bigint',
      'int:normal' => 'int',

      'float:tiny' => 'float',
      'float:small' => 'float',
      'float:medium' => 'float',
      'float:big' => 'double',
      'float:normal' => 'float',
      
      'blob:big' => 'longblob',
      'blob:normal' => 'blob',

      'datetime:normal' => 'datetime',

    );
  }
  return $map;
}

function schema_mysql_schema_type_map() {
  static $map;
  if (!isset($map)) {
    $map = array_flip(schema_mysql_engine_type_map());
  }
  return $map;
}

function schema_mysql_create_table_sql($table) {
  $sql_cols = array();
  foreach ($table['cols'] as $colname => $col) {
    $sql = $colname.' '.schema_engine_type($col, 'mysql');
    if (isset($col['unsigned']) && $col['unsigned']) {
      $sql .= ' UNSIGNED';
    }
    unset($col['unsigned']);
    if (isset($col['length']) && $col['length']) {
      $sql .= '('.$col['length'].')';
    } else if (isset($col['disp-width']) && $col['disp-width']) {
      $sql .= '('.$col['disp-width'].')';
    }
    if (isset($col['type']) && $col['type'] == 'serial') {
      $sql .= ' AUTO_INCREMENT';
    }
    unset($col['type']);
    unset($col['length']);
    unset($col['disp-width']);
    if (isset($col['not null']) && $col['not null']) {
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
  if (is_array($table['primary'])) {
    $sql_keys[] = 'PRIMARY KEY ('.implode(', ', $table['primary']).')';
  }
  foreach (array('unique', 'index') as $type) {
    if (isset($table[$type]) && is_array($table[$type])) {
      foreach ($table[$type] as $keyname => $key) {
        $sql = '';
        if ($type == 'unique') {
            $sql = 'UNIQUE ';
        }
        $sql .= 'KEY '.$keyname.' ';
        $sql .= '('.implode(', ', $key).')';
        $sql_keys[] = $sql;
      }
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
      list($col['type'], $col['size']) = schema_schema_type($matches[1], 'mysql');
      if ($matches[2]) {
        if ($numeric) {
          $col['disp-width'] = $matches[2];
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
    if ($r['INDEX_NAME'] == 'PRIMARY') {
      $type = 'primary';
      $tables[$r['TABLE_NAME']][$type][] = $r['COLUMN_NAME'];
      continue;
    } else if ($r['NON-UNIQUE'] == 0) {
      $type = 'unique';
    } else {
      $type = 'index';
    }
    $tables[$r['TABLE_NAME']][$type][$r['INDEX_NAME']][] = $r['COLUMN_NAME'];
  }

  return $tables;
}

?>