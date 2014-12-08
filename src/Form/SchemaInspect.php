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
    $schema = drupal_get_schema(NULL, TRUE);
    $inspect = schema_dbobject()->inspect();
    foreach ($inspect as $name => $table) {
      $module = isset($schema[$name]['module']) ? $schema[$name]['module'] : 'Unknown';
      if (!isset($build[$module])) {
        $build[$module] = array(
          '#type' => 'fieldset',
          '#access' => TRUE,
          '#title' => check_plain($module),
          '#collapsible' => TRUE,
          '#collapsed' => ($module != 'Unknown'),
          '#weight' => ($module == 'Unknown' ? 0 : $mods[$module]+1),
        );
      }
      $build[$module][$name] = array(
        '#type' => 'markup',
        '#markup' => '<textarea style="width:100%" rows="10">' . check_plain(schema_phpprint_table($name, $table)) . '</textarea>',
      );
    }

    return $build;
  }
}
