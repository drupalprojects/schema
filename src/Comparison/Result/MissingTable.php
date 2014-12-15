<?php
/**
 * @file
 * Contains Drupal\schema\Comparison\Result\MissingTable.
 */

namespace Drupal\schema\Comparison\Result;


class MissingTable {
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

  public function getModule() {
    if (isset($this->schema['module'])) {
      return $this->schema['module'];
    }
    return t('Unknown');
  }
}
