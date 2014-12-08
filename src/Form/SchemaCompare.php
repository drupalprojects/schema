<?php

/**
 * @file
 * Contains \Drupal\schema\Form\SchemaCompare.
 */

namespace Drupal\schema\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class SchemaCompare extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'schema_compare';
  }

  

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $build = array();
  
    $states = array(
      'same' => t('Match'),
      'different' => t('Mismatch'),
      'missing' => t('Missing'),
    );
    $descs = array(
      'same' => t('Tables for which the schema and database agree.'),
      'different' => t('Tables for which the schema and database are different.'),
      'missing' => t('Tables in the schema that are not present in the database.'),
    );
  
    $schema = drupal_get_schema(NULL, TRUE);
    $info = schema_compare_schemas($schema);
  
    // The info array is keyed by state (same/different/missing/extra/warn). For missing,
    // the value is a simple array of table names. For warn, it is a simple array of warnings.
    // Get those out of the way first
    if (isset($info['warn'])) {
      foreach ($info['warn'] as $message) {
        drupal_set_message($message, 'warning');
      }
      unset($info['warn']);
    }
  
    $build['extra'] = array(
      '#type' => 'fieldset',
      '#title' => t('Extra (@count)', array('@count' => isset($info['extra']) ? count($info['extra']) : 0)),
      '#description' => t('Tables in the database that are not present in the schema. This indicates previously installed modules that are disabled but not un-installed or modules that do not use the Schema API.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => 50,
    );
    $build['extra']['tablelist'] = array(
      '#theme' => 'item_list',
      '#items' => isset($info['extra']) ? $info['extra'] : array(),
    );
    unset($info['extra']);
  
    // For the other states, the value is an array keyed by module name. Each value
    // in that array is an array keyed by tablename, and each of those values is an
    // array containing 'status' (same as the state), an array of reasons, and an array of notes.
    $weight = 0;
    foreach ($info as $state => $modules) {
      // We'll fill in the fieldset title below, once we have the counts
      $build[$state] = array(
        '#type' => 'fieldset',
        '#description' => $descs[$state],
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#weight' => $weight++,
      );
      $counts[$state] = 0;
  
      foreach ($modules as $module => $tables) {
        $counts[$state] += count($tables);
        $build[$state][$module] = array(
          '#type' => 'fieldset',
          '#title' => $module,
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
        );
        switch ($state) {
          case 'same':
          case 'missing':
            $build[$state][$module]['tablelist'] = array(
              '#theme' => 'item_list',
              '#items' => array_keys($tables),
            );
            break;
  
          case 'different':
            $items = array();
            foreach ($tables as $name => $stuff) {
              $build[$state][$module][$name] = array(
                '#type' => 'fieldset',
                '#collapsible' => TRUE,
                '#collapsed' => TRUE,
                '#title' => $name,
              );
              $build[$state][$module][$name]['reasons'] = array(
                '#theme' => 'item_list',
                '#items' => array_merge($tables[$name]['reasons'], $tables[$name]['notes']),
              );
            }
            break;
        }
      }
    }
  
    // Fill in counts in titles
    foreach ($states as $state => $description) {
      $build[$state]['#title'] = t('@state (@count)', array('@state' => $states[$state], '@count' => isset($counts[$state]) ? $counts[$state] : 0));
    }
  
    return $build;
  }
}
