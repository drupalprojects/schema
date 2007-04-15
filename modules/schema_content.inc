<?php

function content_schema() {
  $schema = array();
  $fields = array();

  // All CCK content type and field tables share these properties.
  $base = array(
    'cols' => array(
      'vid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1,
        'default' => '0'),
      'nid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1,
        'default' => '0')),
    'keys' => array(
      'PRIMARY' => array('vid'))
    );
  
  $r = db_query('SELECT * FROM {node_field}');
  while ($f = db_fetch_array($r)) { 
    $fields[$f['field_name']] = $f;
    if ($f['db_storage'] != CONTENT_DB_STORAGE_PER_FIELD) {
      continue;
    }

    // Initialize content_field tables.
    $name = 'content_'.$f['field_name'];
    $schema[$name] = $base;
    if ($f['multiple']) {
      $schema[$name]['cols']['delta'] = array(
        'type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => '0');
      $schema[$name]['keys']['PRIMARY'][0] .= ', delta';
    }
    
    _content_schema_add_field($schema, $name, $f);
  }

  // Initialize content_type table entries.
  $r = db_query('SELECT * FROM {node_type}');
  while ($t = db_fetch_array($r)) {
    $schema['content_type_'.$t['type']] = $base;
  }

  // Add field columns to content_type table entries.
  $r = db_query('SELECT * FROM {node_field_instance}');
  while ($fi = db_fetch_array($r)) { 
    $field = $fields[$fi['field_name']];
    if ($field['db_storage'] == CONTENT_DB_STORAGE_PER_CONTENT_TYPE) {
      _content_schema_add_field($schema, 'content_type_'.$fi['type_name'],
        $field);
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
    $sc_col['not null'] = 0;
    unset($sc_col['default']);

    $schema[$tblname]['cols'][$sc_colname] = $sc_col;
  }
}  