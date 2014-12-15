<?php
/**
 * @file
 * Contains Drupal\schema\Comparison\Result\ExtraTable.
 */

namespace Drupal\schema\Comparison\Result;


class ExtraTable {

  protected $table_name;
  protected $schema;

  function __construct($table_name, $schema) {
    $this->table_name = $table_name;
    $this->schema = $schema;
  }

  public function getTableName() {
    return $this->table_name;
  }

  public function getSchema() {
    return $this->schema;
  }
}
