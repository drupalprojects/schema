<?php
/**
 * @file
 * Contains Drupal\schema\Plugin\Schema\System.
 */

namespace Drupal\schema\Plugin\Schema;

use Drupal\Core\Plugin\PluginBase;
use Drupal\schema\SchemaProviderInterface;

/**
 * Provides schema information defined by modules in implementations of
 * hook_schema().
 *
 * @SchemaProvider(id = "system")
 */
class System extends PluginBase implements SchemaProviderInterface {

  /**
   * {@inheritdoc}
   */
  public function get($rebuild = FALSE) {
    return drupal_get_schema(NULL, $rebuild);
  }
}
