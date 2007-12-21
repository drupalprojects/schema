<?php

function db_type_map() {
  return schema_engine_invoke(NULL, 'engine_type_map');
}

/**
 * Get the schema definition of a table, or the whole database
 * schema.  Copied from D6.
 *
 * @param $name
 *   The name of the table. If not given, the schema of all tables is returned.
 * @param $rebuild
 *   If true, the schema will be rebuilt instead of retrieved from the cache.
 */
function drupal_get_schema($name = NULL, $rebuild = FALSE) {
  static $schema = array();

  if (empty($schema) || $rebuild) {
    $schema = array();
    // Load the .install files to get hook_schema.
    module_load_all_includes('install');
    
    // Invoke hook_schema for all modules.
    foreach (module_implements('schema') as $module) {
      $current = module_invoke($module, 'schema');
      _drupal_initialize_schema($module, $current);
      $schema = array_merge($schema, $current);
    }
  }

  if (!isset($name)) {
    return $schema;
  }
  elseif (isset($schema[$name])) {
    return $schema[$name];
  }
  else {
    return FALSE;
  }
}

/**
 * Fill in required default values for table definitions returned by
 * hook_schema().  Copied from D6.
 *
 * @param $module
 *   The module for which hook_schema() was invoked.
 * @param $schema
 *   The schema definition array as it was returned by the module's
 *   hook_schema().
 */
function _drupal_initialize_schema($module, &$schema) {
  // Set the name and module key for all tables.
  foreach ($schema as $name => $table) {
    if (empty($table['module'])) {
      $schema[$name]['module'] = $module;
    }
    if (!isset($table['name'])) {
      $schema[$name]['name'] = $name;
    }
  }
}

/**
 * Return an array of field names from an array of key/index column
 * specifiers.  Copied from D6.
 *
 * This is usually an identity function but if a key/index uses a column prefix
 * specification, this function extracts just the name.
 *
 * @param $fields
 *   An array of key/index column specifiers.
 * @return
 *   An array of field names.
 */
function db_field_names($fields) {
  $ret = array();
  foreach ($fields as $field) {
    if (is_array($field)) {
      $ret[] = $field[0];
    }
    else {
      $ret[] = $field;
    }
  }
  return $ret;
}

/**
 * Load an include file for each of the modules that have been enabled in
 * the system table.  Copied from D6.
 */
function module_load_all_includes($type, $name = NULL) {
  $modules = module_list();
  foreach ($modules as $module) {
    module_load_include($type, $module, $name);
  }
}

/**
 * Load a module include file.  Copied from D6.
 *
 * @param $type
 *   The include file's type (file extension).
 * @param $module
 *   The module to which the include file belongs.
 * @param $name
 *   Optionally, specify the file name. If not set, the module's name is used.
 */
function module_load_include($type, $module, $name = NULL) {
  if (empty($name)) {
    $name = $module;
  }

  $file = './'. drupal_get_path('module', $module) ."/$name.$type";

  if (is_file($file)) {
    require_once $file;
  }
  else {
    return FALSE;
  }
}

if (!function_exists('array_intersect_key'))
{
  function array_intersect_key($isec, $keys)
  {
    $argc = func_num_args();
    if ($argc > 2)
    {
      for ($i = 1; !empty($isec) && $i < $argc; $i++)
      {
        $arr = func_get_arg($i);
        foreach (array_keys($isec) as $key)
        {
          if (!isset($arr[$key]))
          {
            unset($isec[$key]);
          }
        }
      }
      return $isec;
    }
    else
    {
      $res = array();
      foreach (array_keys($isec) as $key)
      {
        if (isset($keys[$key]))
        {
          $res[$key] = $isec[$key];
        }
      }
      return $res;
    }
  }
}

?>
