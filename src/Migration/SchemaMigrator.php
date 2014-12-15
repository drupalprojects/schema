<?php
/**
 * @file
 * Contains Drupal\schema\Migration\SchemaMigrator.
 */

namespace Drupal\schema\Migration;

use Drupal\schema\Comparison\Result\DifferentColumn;
use Drupal\schema\Comparison\Result\ExtraColumn;
use Drupal\schema\Comparison\Result\SchemaComparison;
use Drupal\schema\Comparison\Result\TableComparison;
use Drupal\schema\DatabaseSchemaInspectionInterface;

/**
 * Modifies the database schema to match the declared schema.
 */
class SchemaMigrator {

  /**
   * @var SchemaComparison
   */
  protected $comparison;

  /**
   * @var SchemaMigratorOptions
   */
  protected $options;

  /**
   * @var \Psr\Log\LoggerInterface
   */
  protected $logger;

  /**
   * @param SchemaComparison $comparison
   * @param DatabaseSchemaInspectionInterface $dbschema
   * @param SchemaMigratorOptions $options
   */
  public function __construct(SchemaComparison $comparison, DatabaseSchemaInspectionInterface $dbschema, SchemaMigratorOptions $options = NULL) {
    $this->comparison = $comparison;
    $this->dbschema = $dbschema;

    $this->options = $options;
    if ($this->options == NULL) {
      $this->options = new SchemaMigratorOptions();
    }

    $this->logger = \Drupal::logger('schema');
  }

  public function execute() {
    $tables = $this->comparison->getDifferentTables();
    /** @var TableComparison $table */
    foreach ($tables as $table) {
      if ($this->options()->fixTableComments) {
        $this->fixTableComment($table);
      }
      if ($this->options()->addMissingColumns) {
        throw new \Exception("Adding missing columns not implemented yet.");
      }
      if ($this->options()->updateColumnProperties) {
        $this->updateColumnProperties($table);
      }
      if ($this->options()->removeExtraColumns) {
        $this->removeExtraColumns($table);
      }
    }
  }

  public function options() {
    return $this->options;
  }

  protected function fixTableComment(TableComparison $table) {
    if ($table->isTableCommentDifferent()) {
      $this->dbschema->updateTableComment($table->getTableName(), $table->getDeclaredTableComment());
      $this->logSuccess("Updated comment for {table} to '{comment}'.", array(
        'table' => $table->getTableName(),
        'comment' => $table->getDeclaredTableComment(),
      ));
    }
  }

  protected function logSuccess($message, $context) {
    if (function_exists('drush_log')) {
      drush_log($this->logMessageInterpolate($message, $context), 'success');
    }
    $this->logger->info($message, $context);
  }

  /**
   * Interpolates context values into the message placeholders.
   */
  protected function logMessageInterpolate($message, array $context = array()) {
    // build a replacement array with braces around the context keys
    $replace = array();
    foreach ($context as $key => $val) {
      $replace['{' . $key . '}'] = $val;
    }

    // interpolate replacement values into the message and return
    return strtr($message, $replace);
  }

  /**
   * @param $table TableComparison
   */
  protected function updateColumnProperties($table) {
    if (!empty($differences = $table->getDifferentColumns())) {
      /** @var DifferentColumn $column */
      foreach ($differences as $column) {
        // The schema comparator has already determined that the field exists
        // and that at least some of the properties are different.+
        // @todo Update respective indices at the same time; otherwise this will fail for primary keys.
        $this->dbschema->changeField($column->getTableName(), $column->getColumnName(), $column->getColumnName(), $column->getDeclaredSchema());

        $this->logSuccess("Changed column {table}.{field} definition to {schema}.", array(
          'table' => $column->getTableName(),
          'field' => $column->getColumnName(),
          'schema' => '[' . $this->schemaString($column->getDeclaredSchema()) . ']',
        ));
      }
    }
  }

  protected function schemaString($schema) {
    return implode(', ', array_map(function ($k, $v) {
      return $k . '=' . $v;
    }, array_keys($schema), $schema));
  }

  /**
   * @param $table TableComparison
   */
  protected function removeExtraColumns($table) {
    if (!empty($extra_columns = $table->getExtraColumns())) {
      /** @var ExtraColumn $column */
      foreach ($extra_columns as $column) {
        if ($this->dbschema->dropField($column->getTableName(), $column->getColumnName())) {
          $this->logSuccess("Dropped column {table}.{field}.", array(
            'table' => $column->getTableName(),
            'field' => $column->getColumnName(),
          ));
        }
        else {
          $this->logError("Tried to drop non-existent field {table}.{field}.", array(
            'table' => $column->getTableName(),
            'field' => $column->getColumnName(),
          ));
        }
      }
    }
  }

  protected function logError($message, $context) {
    if (function_exists('drush_log')) {
      drush_log($this->logMessageInterpolate($message, $context), 'error');
    }
    $this->logger->error($message, $context);
  }

}
