<?php
// $Id$

// From content_admin.inc, these are all 'database columns' types that
// fields are allowed to return.  They have MySQL semantics, but we
// cannot depend on the MySQL engine being loaded.
//
// 'int', 'mediumint', 'bigint', 'tinyint', 'float', 'varchar',
// 'text', 'mediumtext', 'longtext'
//
// Eventually, CCK fields should switch to using Schema data types
// directly; then this map and the type_map functions can go away.

function schema_content_engine_type_map() {
  static $map;
  if (!isset($map)) {
    $map = array(
      'varchar:normal' => 'varchar',
      
      //'text:small' => 'smalltext',
      'text:medium' => 'mediumtext',
      'text:big' =>  'longtext',
      'text:normal' => 'text',

      //'serial:normal' => 'int',
      //'serial:big' => 'bigint',

      'int:tiny' => 'tinyint',
      //'int:small' => 'smallint',
      'int:medium' => 'mediumint',
      'int:big' => 'bigint',
      'int:normal' => 'int',

      //'float:tiny' => 'float',
      //'float:small' => 'float',
      //'float:medium' => 'float',
      //'float:big' => 'double',
      'float:normal' => 'float',
      
      //'blob:big' => 'longblog',
      //'blob:normal' => 'blob',

      //'datetime:normal' => 'datetime',
    );
  }
  return $map;
}

function schema_content_schema_type_map() {
  static $map;
  if (!isset($map)) {
    $map = array_flip(schema_content_engine_type_map());
  }
  return $map;
}

function content_schema() {
  $info = _content_type_info();

  $schema = array();
  $fields = array();

  $schema['node_field'] = array(
    'fields' => array(
      'field_name' => array('type' => 'varchar', 'length' => '32', 'not null' => 1, 'default' => ''),
      'type' => array('type' => 'varchar', 'length' => '127', 'not null' => 1, 'default' => ''),
      'global_settings' => array('type' => 'text', 'not null' => 1),
      'required' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'multiple' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'db_storage' => array('type' => 'int', 'not null' => 1, 'default' => 0)),
    'primary key' => array('field_name'),
    );
  $schema['node_field_instance'] = array(
    'fields' => array(
      'field_name' => array('type' => 'varchar', 'length' => '32', 'not null' => 1, 'default' => ''),
      'type_name' => array('type' => 'varchar', 'length' => '32', 'not null' => 1, 'default' => ''),
      'weight' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'label' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'widget_type' => array('type' => 'varchar', 'length' => '32', 'not null' => 1, 'default' => ''),
      'widget_settings' => array('type' => 'text', 'not null' => 1),
      'display_settings' => array('type' => 'text', 'not null' => 1),
      'description' => array('type' => 'text', 'not null' => 1)),
    'primary key' => array('field_name', 'type_name'),
    );

  // All CCK content type and field tables share these properties.
  $base = array(
    'fields' => array(
      'vid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1,
        'default' => '0',
        'join' => array('system', 'node', 'vid'),
        'join-type' => 'one'),
      'nid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1,
        'default' => '0')),
    'primary key' => array('vid'),
    );

  foreach ($info['fields'] as $f) {
    if ($f['db_storage'] == CONTENT_DB_STORAGE_PER_FIELD) {
      $name = 'content_'.$f['field_name'];
      $schema[$name] = $base;
      if ($f['multiple']) {
        $schema[$name]['fields']['delta'] = array(
          'type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => '0');
        $schema[$name]['primary key'][] = 'delta';
        $schema[$name]['fields']['vid']['join-type'] = 'many';
      }
      
      _content_schema_add_field($schema, $name, $f);
    }
  }

  foreach ($info['content types'] as $t_name => $type) {
    $schema['content_type_'.$t_name] = $base;
    $schema['content_type_'.$t_name]['fields']['vid']['node-types'][]=$t_name;
    foreach ($type['fields'] as $f_name => $field) {
      if ($field['db_storage'] == CONTENT_DB_STORAGE_PER_CONTENT_TYPE) {
        _content_schema_add_field($schema, 'content_type_'.$t_name,
          $field);
      } else {
        // The field does not add columns to this type's table but
        // does join to nodes of this type.
        $schema['content_'.$f_name]['fields']['vid']['node-types'][]=$t_name;
      }
    }
  }

  return $schema;
}

function _content_schema_add_field(&$schema, $tblname, $f) {
  $field_cols = module_invoke($f['type'], 'field_settings', 
    'database columns', $f);
  if (! isset($field_cols)) {
    // XXX Is this right?  Sometimes $f['type'] is the module name and
    // sometimes it isn't but $f['widget']['type'] is, and vice versa.
    $field_cols = module_invoke($f['widget']['type'], 'field_settings', 
      'database columns', $f);
  }

  foreach ($field_cols as $f_colname => $f_col) {
    $sc_colname = $f['field_name'].'_'.$f_colname;
    $sc_col = $f_col;

    // normalize CCK field attrs to Schema standards
    if (is_null($sc_col['default'])) {
      unset($sc_col['default']);
    }
    else if ($sc_col['default'] === "''") {
      $sc_col['default'] = '';
    }
    if (isset($sc_col['not null'])) {
      $sc_col['not null'] = ($sc_col['not null'] ? 1 : 0);
    }

    list($sc_col['type'], $sc_col['size']) = schema_schema_type(
      $sc_col['type'], 'content');

    // Convert CCK field 'length' for int fields into 'disp_width'
    if ($sc_col['type'] == 'int' && isset($sc_col['length'])) {
      $sc_col['disp_width'] = $sc_col['length'];
      unset($sc_col['length']);
    }

    $schema[$tblname]['fields'][$sc_colname] = $sc_col;
  }
}  