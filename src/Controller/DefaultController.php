<?php /**
 * @file
 * Contains \Drupal\schema\Controller\DefaultController.
 */

namespace Drupal\schema\Controller;

use Drupal\Core\Controller\ControllerBase;

/**
 * Default controller for the schema module.
 */
class DefaultController extends ControllerBase {


  public function schema_sql($engine = NULL) {
    $schema = drupal_get_schema(NULL, TRUE);
    $connection = Database::getConnection();
    $sql = '';
    foreach ($schema as $name => $table) {
      if (substr($name, 0, 1) == '#') {
        continue;
      }
      if ($engine) {
        $stmts = call_user_func('schema_' . $engine . '_create_table_sql', $table);
      }
      else {
        $stmts = schema_dbobject()->getCreateTableSql($name, $table);
      }
  
      $sql .= implode(";\n", $stmts) . ";\n\n";
    }
  
    return "<textarea style=\"width:100%\" rows=\"30\">$sql</textarea>";
  }

  public function schema_show() {
    $schema = drupal_get_schema(NULL, TRUE);
    $show = var_export($schema, 1);
  
    return "<textarea style=\"width:100%\" rows=\"30\">$show</textarea>";
  }
}
