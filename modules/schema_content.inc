<?php
// $Id$

function content_schema() {
  $info = _content_type_info();

  $schema = array();
  $fields = array();

  // All CCK content type and field tables share these properties.
  $base = array(
    'cols' => array(
      'vid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1,
        'default' => '0',
        'join' => array('system', 'node', 'vid'),
        'join-type' => 'one'),
      'nid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1,
        'default' => '0')),
    'keys' => array(
      'PRIMARY' => array('vid'))
    );

  foreach ($info['fields'] as $f) {
    if ($f['db_storage'] == CONTENT_DB_STORAGE_PER_FIELD) {
      $name = 'content_'.$f['field_name'];
      $schema[$name] = $base;
      if ($f['multiple']) {
        $schema[$name]['cols']['delta'] = array(
          'type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => '0');
        $schema[$name]['keys']['PRIMARY'][0] .= ', delta';
        $schema[$name]['cols']['vid']['join-type'] = 'many';
      }
      
      _content_schema_add_field($schema, $name, $f);
    }
  }

  foreach ($info['content types'] as $t_name => $type) {
    $schema['content_type_'.$t_name] = $base;
    foreach ($type['fields'] as $f_name => $field) {
      if ($field['db_storage'] == CONTENT_DB_STORAGE_PER_CONTENT_TYPE) {
        _content_schema_add_field($schema, 'content_type_'.$t_name,
          $field);
      }
    }
  }

  return $schema;
}

function _content_schema_add_field(&$schema, $tblname, $f) {
  $field_cols = module_invoke($f['type'], 'field_settings', 
    'database columns', $f);
  foreach ($field_cols as $f_colname => $f_col) {
    $sc_colname = $f['field_name'].'_'.$f_colname;
    $sc_col = $f_col;

    // XXX CCK (at least as of 5.x) appears not to maintain
    // the nullable and default properties of columns according to the
    // 'database columns' specs returned by modules.  Detecting these
    // errors is a major benefit of Schema's inspection and comparison
    // features but, during developing, I don't want to be distracted
    // by mismatches which aren't my fault!  So, for now, I make the
    // column specifications match what CCK actually does instead of
    // what it is supposed to do.
    if ($f_colname != 'format') {
      $sc_col['not null'] = 0;
      unset($sc_col['default']);
    }

    // Convert CCK field 'length' for int fields into 'disp_width'
    if (substr($sc_col['type'], 'int') !== FALSE && isset($sc_col['length'])){
      $sc_col['disp_width'] = $sc_col['length'];
      unset($sc_col['length']);
    }

    $schema[$tblname]['cols'][$sc_colname] = $sc_col;
  }
}  