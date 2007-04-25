<?php
// $Id$

function _schema_mysql_type_map() {
  static $map;
  if (!isset($map)) {
    $map = array(
      'varchar:normal' => 'VARCHAR',
      
      'text:small' => 'SMALLTEXT',
      'text:medium' => 'MEDIUMTEXT',
      'text:big' =>  'LONGTEXT',
      'text:normal' => 'TEXT',

      'serial:normal' => 'INT',
      'serial:big' => 'BIGINT',

      'int:tiny' => 'TINYINT',
      'int:small' => 'SMALLINT',
      'int:medium' => 'MEDIUMINT',
      'int:big' => 'BIGINT',
      'int:normal' => 'INT',

      'float:tiny' => 'FLOAT',
      'float:small' => 'FLOAT',
      'float:medium' => 'FLOAT',
      'float:big' => 'DOUBLE',
      'float:normal' => 'FLOAT',
      
      'blob:big' => 'LONGBLOG',
      'blob:normal' => 'BLOB',

      'datetime:normal' => 'DATETIME',

    );
  }
  return $map;
}

function _schema_type_to_mysql($col) {
  $map = _schema_mysql_type_map();
  $size = (isset($col['size']) ? $col['size'] : 'normal');
  $type = $col['type'].':'.$size;
  if (isset($map[$type])) {
    return $map[$type];
  } else {
    drupal_set_message(t('No MySQL type for Schema type %type.',
                         array('%type' => $type)), 'error');
    return $col['type'];
  }
}

function _schema_mysql_to_type($type) {
  $map = array_flip(_schema_mysql_type_map()); // inefficient
  if (isset($map[$type])) {
    return explode(':', $map[$type]);
  } else {
    drupal_set_message(t('No Schema type for MySQL type %type.',
                         array('%type' => $type)), 'error');
    return array($type, 'normal');
  }
}

function schema_mysql_create_table_sql($table) {
  $sql_cols = array();
  foreach ($table['cols'] as $colname => $col) {
    $sql = $colname.' '._schema_type_to_mysql($col);
    if ($col['unsigned']) {
      $sql .= ' UNSIGNED';
    }
    unset($col['unsigned']);
    if ($col['length']) {
      $sql .= '('.$col['length'].')';
    }
    if ($col['type'] == 'serial') {
      $sql .= ' AUTO_INCREMENT';
    }
    unset($col['type']);
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
  if (is_array($table['primary'])) {
    $sql_keys[] = 'PRIMARY KEY ('.implode(', ', $table['primary']).')';
  }
  foreach (array('unique', 'index') as $type) {
    if (is_array($table[$type])) {
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
    if ($r['INDEX_NAME'] == 'PRIMARY') {
      $type = 'primary';
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