<?php
/**
 * @file
 * Contains Drupal\schema\Comparison\ColumnDifference.
 */

namespace Drupal\schema\Comparison\Result;


class DifferentColumn {
  protected $column_name;
  protected $table_name;
  protected $different_keys;
  protected $declared_schema;
  protected $actual_schema;

  public function __construct($table_name, $column_name, $different_keys, $declared_schema, $actual_schema) {
    $this->actual_schema = $actual_schema;
    $this->column_name = $column_name;
    $this->declared_schema = $declared_schema;
    $this->different_keys = $different_keys;
    $this->table_name = $table_name;
  }

  public function getActualSchema() {
    return $this->actual_schema;
  }

  public function getColumnName() {
    return $this->column_name;
  }

  public function getDeclaredSchema() {
    return $this->declared_schema;
  }

  public function getDifferentKeys() {
    return $this->different_keys;
  }

  public function getTableName() {
    return $this->table_name;
  }

  public function getModule() {
    if (isset($this->declared_schema['module'])) {
      return $this->declared_schema['module'];
    }
    return t('Unknown');
  }
}
