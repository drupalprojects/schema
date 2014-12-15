<?php
/**
 * @file
 * Contains Drupal\schema\Comparison\Result\MissingColumn.
 */

namespace Drupal\schema\Comparison\Result;


class MissingColumn {

  protected $column_name;
  protected $table_name;
  protected $schema;

  function __construct($table_name, $column_name, $schema) {
    $this->table_name = $table_name;
    $this->column_name = $column_name;
    $this->schema = $schema;
  }

  public function getTableName() {
    return $this->table_name;
  }

  public function getColumnName() {
    return $this->column_name;
  }

  public function getSchema() {
    return $this->schema;
  }
}
