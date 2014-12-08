<?php

/**
 * @file
 * Contains \Drupal\schema\Form\SchemaDescribe.
 */

namespace Drupal\schema\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

class SchemaDescribe extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'schema_describe';
  }

  

  public function buildForm(array $form, \Drupal\Core\Form\FormStateInterface $form_state) {
    $build = array();
  
    $schema = drupal_get_schema(NULL, TRUE);
    ksort($schema);
    $row_hdrs = array(t('Name'), t('Type[:Size]'), t('Null?'), t('Default'));
  
    $default_table_description = t('TODO: please describe this table!');
    $default_field_description = t('TODO: please describe this field!');
    foreach ($schema as $t_name => $t_spec) {
      $rows = array();
      foreach ($t_spec['fields'] as $c_name => $c_spec) {
        $row = array();
        $row[] = $c_name;
        $type = $c_spec['type'];
        if (!empty($c_spec['length'])) {
          $type .= '(' . $c_spec['length'] . ')';
        }
        if (!empty($c_spec['scale']) && !empty($c_spec['precision'])) {
          $type .= '(' . $c_spec['precision'] . ', ' . $c_spec['scale'] . ' )';
        }
        if (!empty($c_spec['size']) && $c_spec['size'] != 'normal') {
          $type .= ':' . $c_spec['size'];
        }
        if ($c_spec['type'] == 'int' && !empty($c_spec['unsigned'])) {
          $type .= ', unsigned';
        }
        $row[] = $type;
        $row[] = !empty($c_spec['not null']) ? 'NO' : 'YES';
        $row[] = isset($c_spec['default']) ? (is_string($c_spec['default']) ? '\'' . $c_spec['default'] . '\'' : $c_spec['default']) : '';
        $rows[] = $row;
        if (!empty($c_spec['description']) && $c_spec['description'] != $default_field_description) {
          $desc = _schema_process_description($c_spec['description']);
          $rows[] = array(array('colspan' => count($row_hdrs), 'data' => $desc));
        }
        else {
          drupal_set_message(_schema_process_description(t('Field {!table}.@field has no description.', array('!table' => $t_name, '@field' => $c_name))), 'warning');
        }
      }
  
      if (empty($t_spec['description']) || $t_spec['description'] == $default_table_description) {
        drupal_set_message(_schema_process_description(t('Table {!table} has no description.', array('!table' => $t_name))), 'warning');
      }
  
      $build[$t_name] = array(
        '#type' => 'fieldset',
        '#title' => t('@table (@module module)',
          array('@table' => $t_name, '@module' => isset($t_spec['module']) ? $t_spec['module'] : '')),
        '#description' => !empty($t_spec['description']) ? _schema_process_description($t_spec['description']) : '',
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#attributes' => array('id' => 'table-' . $t_name),
      );
      $build[$t_name]['content'] = array(
        '#theme' => 'table',
        '#header' => $row_hdrs,
        '#rows' => $rows,
      );
    }
  
    return $build;
  }
}
