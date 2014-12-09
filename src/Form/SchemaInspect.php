<?php

/**
 * @file
 * Contains \Drupal\schema\Form\SchemaInspect.
 */

namespace Drupal\schema\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class SchemaInspect extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'schema_inspect';
  }

  /**
   * Form submission handler.
   *
   * @param array $form
   *   An associative array containing the structure of the form.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The current state of the form.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // TODO: Implement submitForm() method.
  }


  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $build = array();

    $mods = module_list();
    sort($mods);
    $mods = array_flip($mods);
    $schema = schema_get_schema(TRUE);
    $inspect = schema_dbobject()->inspect();
    foreach ($inspect as $name => $table) {
      $module = isset($schema[$name]['module']) ? $schema[$name]['module'] : 'Unknown';
      if (!isset($build[$module])) {
        $build[$module] = array(
          '#type' => 'details',
          '#title' => $module,
          '#open' => $module == 'Unknown',
          '#weight' => ($module == 'Unknown' ? 0 : $mods[$module]+1),
        );
      }
      $build[$module][$name] = array(
        '#type' => 'textarea',
        '#rows' => 10,
        '#default_value' => schema_phpprint_table($name, $table),
        '#attributes' => array(
          'style' => 'width:100%;'
        )
      );
    }

    return $build;
  }
}
