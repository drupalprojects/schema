<?php
// $Id$

/**
 * @file
 * MySQL implementation of schema module hooks.
 *
 */

/**
 *
 * @return unknown_type
 */
function schema_mysql_engine_type_map() {
  $map = db_type_map();
  return $map;
}

function schema_mysql_schema_type_map() {
  static $map;
  if (!isset($map)) {
    $map = array_flip(array_map('strtolower', schema_mysql_engine_type_map()));
  }
  return $map;
}

function schema_mysql_create_table_sql($table) {
  $sql_cols = array();
  foreach ($table['fields'] as $colname => $col) {
    $sql = $colname . ' ' . schema_engine_type($col, 'mysql');
    if (isset($col['unsigned']) && $col['unsigned']) {
      $sql .= ' UNSIGNED';
    }
    unset($col['unsigned']);
    if (isset($col['length']) && $col['length']) {
      $sql .= '(' . $col['length'] . ')';
    }
    if (isset($col['type']) && $col['type'] == 'serial') {
      $sql .= ' AUTO_INCREMENT';
    }
    unset($col['type']);
    unset($col['length']);

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
          }
          else {
            $sql .= $val;
          }
          break;
      }
    }
    $sql_cols[] = $sql;
  }

  $sql_keys = array();
  if (is_array($table['primary key'])) {
    $sql_keys[] = 'PRIMARY KEY (' . implode(', ', $table['primary key']) . ')';
  }
  foreach (array('unique keys', 'indexes') as $type) {
    if (isset($table[$type]) && is_array($table[$type])) {
      foreach ($table[$type] as $keyname => $key) {
        $sql = '';
        if ($type == 'unique keys') {
            $sql = 'UNIQUE ';
        }
        $sql .= 'KEY ' . $keyname . ' ';
        $sql .= '(' . implode(', ', $key) . ')';
        $sql_keys[] = $sql;
      }
    }
  }

  $sql = "CREATE TABLE {" . $table['name'] . "} (\n\t";
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

  // Get the current database name
  $info = Database::getConnectionInfo();
  $database = $info['default']['database'];

  $tables = array();
  $args = array(':database' => $database);
  $sql = 'SELECT table_name, table_comment
          FROM information_schema.tables
          WHERE table_schema=:database ';
  if (isset($name)) {
    $sql .= 'AND table_name = :table ';
    $args[':table'] = $name;
  }
  $res = db_query($sql, $args);
  foreach ($res as $r) {
    $tables[$r->table_name]['description'] = $r->table_comment;
  }

  $sql = 'SELECT table_name, column_type, column_name, column_default,
                 extra, is_nullable, numeric_scale, column_comment
          FROM information_schema.columns
          WHERE table_schema=:database ';
  if (isset($name)) {
    $sql .= 'AND table_name = :table ';
  }
  $sql .= 'ORDER BY table_name, ordinal_position';

  $res = db_query($sql, $args);
  foreach ($res as $r) {
    $r->new_table_name = schema_unprefix_table($r->table_name);

    $numeric = !is_null($r->numeric_scale);
    $col = array();
    $col['type'] = $r->column_type;
    if (preg_match('@([a-z]+)(?:\((\d+)(?:,(\d+))?\))?\s*(unsigned)?@', $col['type'], $matches)) {
      list($col['type'], $col['size']) = schema_schema_type($matches[1], $r->table_name, $r->column_name, 'mysql');
      if (isset($matches[2])) {
        if ($col['type'] == 'numeric' || $col['type'] == 'float' || $col['type'] == 'double') {
          $col['precision'] = $matches[2];
          $col['scale'] = $matches[3];
        }
        elseif (!$numeric) {
          $col['length'] = $matches[2];
        }
      }
      if (isset($matches[4])) {
        $col['unsigned'] = TRUE;
      }
    }
    if ($col['type'] == 'int' && isset($r->extra) &&
        $r->extra == 'auto_increment') {
      $col['type'] = 'serial';
    }
    $col['not null'] = ($r->is_nullable == 'YES' ? FALSE : TRUE);
    if (! is_null($r->column_default)) {
      if ($numeric) {
        // XXX floats!
        $col['default'] = intval($r->column_default);
      }
      else {
        $col['default'] = $r->column_default;
      }
    }
    $col['description'] = $r->column_comment;
    $tables[$r->table_name]['fields'][$r->column_name] = $col;
    // At this point, $tables is indexed by the raw db table name - save the unprefixed
    // name for later use
    $tables[$r->table_name]['name'] = $r->new_table_name;
  }

  $sql = 'SELECT table_name, column_name, index_name, sub_part, non_unique
          FROM information_schema.statistics
          WHERE table_schema=:database ';
  if (isset($name)) {
    $sql .= 'AND table_name = :table ';
  }
  $sql .= 'ORDER BY table_name, index_name, seq_in_index';

  $res = db_query($sql, $args);
  foreach ($res as $r) {
    if (isset($r->sub_part) && !is_null($r->sub_part)) {
      $col = array($r->column_name, intval($r->sub_part));
    }
    else {
      $col = $r->column_name;
    }
    if ($r->index_name == 'PRIMARY') {
      $type = 'primary key';
      $tables[$r->table_name][$type][] = $col;
      continue;
    }
    elseif ($r->non_unique == 0) {
      $type = 'unique keys';
    }
    else {
      $type = 'indexes';
    }
    $tables[$r->table_name][$type][$r->index_name][] = $col;
  }

  // Now, for tables which we have unprefixed, index $tables by the unprefixed name
  foreach ($tables as $tablename => $table) {
    $newname = $tables[$tablename]['name'];
    if ($tablename != $newname) {
      $tables[$newname] = $table;
      unset($tables[$tablename]);
    }
  }

  return $tables;
}
