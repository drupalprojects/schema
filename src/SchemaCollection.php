<?php
/**
 * @file
 * Contains Drupal\schema\SchemaCollection.
 */

namespace Drupal\schema;

use Drupal\Core\Plugin\DefaultLazyPluginCollection;

class SchemaCollection extends DefaultLazyPluginCollection {

  /**
   * {@inheritdoc}
   *
   * @return \Drupal\schema\SchemaProviderInterface
   */
  public function &get($instance_id) {
    return parent::get($instance_id);
  }
}
