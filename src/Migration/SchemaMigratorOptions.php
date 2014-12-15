<?php
/**
 * @file
 * Contains Drupal\schema\Migration\SchemaMigratorOptions.
 */

namespace Drupal\schema\Migration;


class SchemaMigratorOptions {
  public $addMissingColumns = FALSE;
  public $updateColumnProperties = FALSE;
  public $removeExtraColumns = FALSE;
  public $fixTableComments = FALSE;

  /**
   * @param $fixTableComments
   * @return $this
   */
  public function setFixTableComments($fixTableComments) {
    $this->fixTableComments = $fixTableComments;
    return $this;
  }

  /**
   * @param $addMissingColumns
   * @return $this
   */
  public function setAddMissingColumns($addMissingColumns) {
    $this->addMissingColumns = $addMissingColumns;
    return $this;
  }

  /**
   * @param $removeExtraColumns
   * @return $this
   */
  public function setRemoveExtraColumns($removeExtraColumns) {
    $this->removeExtraColumns = $removeExtraColumns;
    return $this;
  }

  /**
   * @param $updateColumnProperties
   * @return $this
   */
  public function setUpdateColumnProperties($updateColumnProperties) {
    $this->updateColumnProperties = $updateColumnProperties;
    return $this;
  }

}
