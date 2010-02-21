<?php
// $Id$

/**
 * @file
 * Schema module enhancements to DatabaseSchema_mysql
 */

class SchemaDatabaseSchema_mysql extends DatabaseSchema_mysql {
  /**
   * Retrieve generated SQL to create a new table from a Drupal schema definition.
   *
   * @param $name
   *   The name of the table to create.
   * @param $table
   *   A Schema API table definition array.
   * @return
   *   An array of SQL statements to create the table.
   */
  public function getCreateTableSql($name, $table) {
    return parent::createTableSql($name, $table);
  }
}
