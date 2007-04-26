<?php
// $Id$

// Until Drupal core uses Schema directly and maintains its own
// system_schema() function, I want it to be easy to generate new
// system schemas directly from the database via inspection every time
// there is an update and drop them into this copy of
// system_schema().  I also want to maintain relationship information
// for core tables but I do not want to have to re-enter every time I
// paste in a new schema array.  Therefore, I maintain the
// relationship information (which changes very infrequently) in a
// separate array and merge them into the return value of
// system_schema().  It's a hack, but it achieves the goal.
function _system_schema_merge($schema) {
  
  // I haven't defined many relationships yet...
  $join_node_1 = array(
    'join' => array('system', 'node', 'nid'),
    'join-type' => 'one');
  $join_node_n = array(
    'join' => array('system', 'node', 'nid'),
    'join-type' => 'many');
  
  $joins_node_1 = array('node_comment_statistics');
  $joins_node_n = array('comments');
  
  foreach ($joins_node_1 as $t_name) {
    $merge[$t_name]['cols']['nid'] = $join_node_1;
  }
  foreach ($joins_node_n as $t_name) {
    $merge[$t_name]['cols']['nid'] = $join_node_n;
  }

  $merge['node']['cols']['uid'] = array(
    'join' => array('system', 'users', 'uid'),
    'join-type' => 'one');
  $merge['node_revisions']['cols']['vid'] = array(
    'join' => array('system', 'node', 'vid'),
    'join-type' => 'one');

  foreach (array_keys($merge) as $t_name) {
    foreach ($merge[$t_name]['cols'] as $c_name => $attrs) {
      $schema[$t_name]['cols'][$c_name] = array_merge(
        $schema[$t_name]['cols'][$c_name], $attrs);
    }
  }
  return $schema;
}

function system_schema()
{
  $schema['#version'] = '1022';
  $schema['#inspect-engine'] = 'pgsql';

  $schema['access'] = array(
    'cols' => array(
      'aid' => array('type' => 'serial', 'not null' => 1),
      'mask' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'type' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'status' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0)),
    'primary' => array('aid'),
    );
  $schema['authmap'] = array(
    'cols' => array(
      'aid' => array('type' => 'serial', 'not null' => 1),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'authname' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'module' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => '')),
    'primary' => array('aid'),
    'unique' => array(
      'authmap_authname_key' => array('authname')),
    );
  $schema['blocks'] = array(
    'cols' => array(
      'module' => array('type' => 'varchar', 'length' => '64', 'not null' => 1, 'default' => ''),
      'delta' => array('type' => 'varchar', 'length' => '32', 'not null' => 1, 'default' => '0'),
      'theme' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'status' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'weight' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'region' => array('type' => 'varchar', 'length' => '64', 'not null' => 1, 'default' => 'left'),
      'custom' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'throttle' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'visibility' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'pages' => array('type' => 'text', 'not null' => 1, 'default' => ''),
      'title' => array('type' => 'varchar', 'length' => '64', 'not null' => 1, 'default' => '')),
    );
  $schema['blocks_roles'] = array(
    'cols' => array(
      'module' => array('type' => 'varchar', 'length' => '64', 'not null' => 1),
      'delta' => array('type' => 'varchar', 'length' => '32', 'not null' => 1),
      'rid' => array('type' => 'int', 'not null' => 1)),
    'primary' => array('module', 'delta', 'rid'),
    );
  $schema['boxes'] = array(
    'cols' => array(
      'bid' => array('type' => 'serial', 'not null' => 1),
      'body' => array('type' => 'text', 'not null' => 0),
      'info' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'format' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0)),
    'primary' => array('bid'),
    'unique' => array(
      'boxes_info_key' => array('info')),
    );
  $schema['cache'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'data' => array('type' => 'blob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'primary' => array('cid'),
    'index' => array(
      'cache_expire_idx' => array('expire')),
    );
  $schema['cache_content'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'data' => array('type' => 'blob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'primary' => array('cid'),
    'index' => array(
      'cache_content_expire_idx' => array('expire')),
    );
  $schema['cache_filter'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'data' => array('type' => 'blob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'primary' => array('cid'),
    'index' => array(
      'cache_filter_expire_idx' => array('expire')),
    );
  $schema['cache_menu'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'data' => array('type' => 'blob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'primary' => array('cid'),
    'index' => array(
      'cache_menu_expire_idx' => array('expire')),
    );
  $schema['cache_page'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'data' => array('type' => 'blob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'primary' => array('cid'),
    'index' => array(
      'cache_page_expire_idx' => array('expire')),
    );
  $schema['comments'] = array(
    'cols' => array(
      'cid' => array('type' => 'serial', 'not null' => 1),
      'pid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'nid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'subject' => array('type' => 'varchar', 'length' => '64', 'not null' => 1, 'default' => ''),
      'comment' => array('type' => 'text', 'not null' => 1),
      'hostname' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'score' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'status' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0),
      'format' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'thread' => array('type' => 'varchar', 'length' => '255', 'not null' => 1),
      'users' => array('type' => 'text', 'not null' => 0),
      'name' => array('type' => 'varchar', 'length' => '60', 'not null' => 0),
      'mail' => array('type' => 'varchar', 'length' => '64', 'not null' => 0),
      'homepage' => array('type' => 'varchar', 'length' => '255', 'not null' => 0)),
    'primary' => array('cid'),
    'index' => array(
      'comments_nid_idx' => array('nid')),
    );
  $schema['file_revisions'] = array(
    'cols' => array(
      'fid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'vid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'description' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'list' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0)),
    'primary' => array('fid', 'vid'),
    'index' => array(
      'file_revisions_vid_idx' => array('vid')),
    );
  $schema['files'] = array(
    'cols' => array(
      'fid' => array('type' => 'serial', 'not null' => 1),
      'nid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'filename' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'filepath' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'filemime' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'filesize' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0)),
    'primary' => array('fid'),
    'index' => array(
      'files_nid_idx' => array('nid')),
    );
  $schema['filter_formats'] = array(
    'cols' => array(
      'format' => array('type' => 'serial', 'not null' => 1),
      'name' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'roles' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'cache' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0)),
    'primary' => array('format'),
    'unique' => array(
      'filter_formats_name_key' => array('name')),
    );
  $schema['filters'] = array(
    'cols' => array(
      'format' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'module' => array('type' => 'varchar', 'length' => '64', 'not null' => 1, 'default' => ''),
      'delta' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'weight' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0)),
    'index' => array(
      'filters_weight_idx' => array('weight')),
    );
  $schema['flood'] = array(
    'cols' => array(
      'event' => array('type' => 'varchar', 'length' => '64', 'not null' => 1, 'default' => ''),
      'hostname' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0)),
    );
  $schema['history'] = array(
    'cols' => array(
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'nid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0)),
    'primary' => array('uid', 'nid'),
    );
  $schema['menu'] = array(
    'cols' => array(
      'mid' => array('type' => 'serial', 'not null' => 1),
      'pid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'path' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'title' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'description' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'weight' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'type' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0)),
    'primary' => array('mid'),
    );
  $schema['node'] = array(
    'cols' => array(
      'nid' => array('type' => 'serial', 'not null' => 1),
      'vid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'type' => array('type' => 'varchar', 'length' => '32', 'not null' => 1, 'default' => ''),
      'title' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'status' => array('type' => 'int', 'not null' => 1, 'default' => 1),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'changed' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'comment' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'promote' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'moderate' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'sticky' => array('type' => 'int', 'not null' => 1, 'default' => 0)),
    'primary' => array('nid', 'vid'),
    'unique' => array(
      'node_vid_key' => array('vid')),
    'index' => array(
      'node_nid_idx' => array('nid'),
      'node_node_changed_idx' => array('changed'),
      'node_node_created_idx' => array('created'),
      'node_node_moderate_idx' => array('moderate'),
      'node_node_promote_status_idx' => array('promote', 'status'),
      'node_node_status_type_idx' => array('status', 'type', 'nid'),
      'node_node_title_type_idx' => array('title'),
      'node_status_idx' => array('status'),
      'node_uid_idx' => array('uid')),
    );
  $schema['node_access'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'gid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'realm' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'grant_view' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0),
      'grant_update' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0),
      'grant_delete' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0)),
    'primary' => array('nid', 'gid', 'realm'),
    );
  $schema['node_comment_statistics'] = array(
    'cols' => array(
      'nid' => array('type' => 'serial', 'not null' => 1),
      'last_comment_timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'last_comment_name' => array('type' => 'varchar', 'length' => '60', 'not null' => 0),
      'last_comment_uid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'comment_count' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0)),
    'primary' => array('nid'),
    'index' => array(
      'node_comment_statistics_node_comment_timestamp_idx' => array('last_comment_timestamp')),
    );
  $schema['node_counter'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'totalcount' => array('type' => 'int', 'unsigned' => 1, 'size' => 'big', 'not null' => 1, 'default' => 0),
      'daycount' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'timestamp' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0)),
    'primary' => array('nid'),
    'index' => array(
      'node_counter_daycount_idx' => array('daycount'),
      'node_counter_timestamp_idx' => array('timestamp'),
      'node_counter_totalcount_idx' => array('totalcount')),
    );
  $schema['node_revisions'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'not null' => 1),
      'vid' => array('type' => 'serial', 'not null' => 1),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'title' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'body' => array('type' => 'text', 'not null' => 1, 'default' => ''),
      'teaser' => array('type' => 'text', 'not null' => 1, 'default' => ''),
      'log' => array('type' => 'text', 'not null' => 1, 'default' => ''),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'format' => array('type' => 'int', 'not null' => 1, 'default' => 0)),
    'primary' => array('vid'),
    'index' => array(
      'node_revisions_nid_idx' => array('nid'),
      'node_revisions_uid_idx' => array('uid')),
    );
  $schema['node_type'] = array(
    'cols' => array(
      'type' => array('type' => 'varchar', 'length' => '32', 'not null' => 1),
      'name' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'module' => array('type' => 'varchar', 'length' => '255', 'not null' => 1),
      'description' => array('type' => 'text', 'not null' => 1),
      'help' => array('type' => 'text', 'not null' => 1),
      'has_title' => array('type' => 'int', 'size' => 'small', 'not null' => 1),
      'title_label' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'has_body' => array('type' => 'int', 'size' => 'small', 'not null' => 1),
      'body_label' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'min_word_count' => array('type' => 'int', 'size' => 'small', 'not null' => 1),
      'custom' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'modified' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'locked' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'orig_type' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => '')),
    'primary' => array('type'),
    );
  $schema['permission'] = array(
    'cols' => array(
      'rid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'perm' => array('type' => 'text', 'not null' => 0),
      'tid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0)),
    'index' => array(
      'permission_rid_idx' => array('rid')),
    );
  $schema['role'] = array(
    'cols' => array(
      'rid' => array('type' => 'serial', 'not null' => 1),
      'name' => array('type' => 'varchar', 'length' => '64', 'not null' => 1, 'default' => '')),
    'primary' => array('rid'),
    'unique' => array(
      'role_name_key' => array('name')),
    );
  $schema['sessions'] = array(
    'cols' => array(
      'uid' => array('type' => 'int', 'not null' => 1),
      'sid' => array('type' => 'varchar', 'length' => '64', 'not null' => 1, 'default' => ''),
      'hostname' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'cache' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'session' => array('type' => 'text', 'not null' => 0)),
    'primary' => array('sid'),
    'index' => array(
      'sessions_timestamp_idx' => array('timestamp'),
      'sessions_uid_idx' => array('uid')),
    );
  $schema['system'] = array(
    'cols' => array(
      'filename' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'name' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'type' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'description' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'status' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'throttle' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'bootstrap' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'schema_version' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => -1),
      'weight' => array('type' => 'int', 'not null' => 1, 'default' => 0)),
    'primary' => array('filename'),
    'index' => array(
      'system_weight_idx' => array('weight')),
    );
  $schema['term_data'] = array(
    'cols' => array(
      'tid' => array('type' => 'serial', 'not null' => 1),
      'vid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'name' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'description' => array('type' => 'text', 'not null' => 0),
      'weight' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0)),
    'primary' => array('tid'),
    'index' => array(
      'term_data_vid_idx' => array('vid')),
    );
  $schema['term_hierarchy'] = array(
    'cols' => array(
      'tid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'parent' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0)),
    'primary' => array('tid', 'parent'),
    'index' => array(
      'term_hierarchy_parent_idx' => array('parent'),
      'term_hierarchy_tid_idx' => array('tid')),
    );
  $schema['term_node'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'tid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0)),
    'primary' => array('tid', 'nid'),
    'index' => array(
      'term_node_nid_idx' => array('nid'),
      'term_node_tid_idx' => array('tid')),
    );
  $schema['term_relation'] = array(
    'cols' => array(
      'tid1' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'tid2' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0)),
    'index' => array(
      'term_relation_tid1_idx' => array('tid1'),
      'term_relation_tid2_idx' => array('tid2')),
    );
  $schema['term_synonym'] = array(
    'cols' => array(
      'tid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'name' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => '')),
    'index' => array(
      'term_synonym_tid_idx' => array('tid')),
    );
  $schema['url_alias'] = array(
    'cols' => array(
      'pid' => array('type' => 'serial', 'not null' => 1),
      'src' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'dst' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => '')),
    'primary' => array('pid'),
    'unique' => array(
      'url_alias_dst_key' => array('dst')),
    'index' => array(
      'url_alias_src_idx' => array('src')),
    );
  $schema['users'] = array(
    'cols' => array(
      'uid' => array('type' => 'serial', 'not null' => 1),
      'name' => array('type' => 'varchar', 'length' => '60', 'not null' => 1, 'default' => ''),
      'pass' => array('type' => 'varchar', 'length' => '32', 'not null' => 1, 'default' => ''),
      'mail' => array('type' => 'varchar', 'length' => '64', 'not null' => 0, 'default' => ''),
      'mode' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'sort' => array('type' => 'int', 'size' => 'small', 'not null' => 0, 'default' => 0),
      'threshold' => array('type' => 'int', 'size' => 'small', 'not null' => 0, 'default' => 0),
      'theme' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'signature' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'access' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'login' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'status' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0),
      'timezone' => array('type' => 'varchar', 'length' => '8', 'not null' => 0),
      'language' => array('type' => 'varchar', 'length' => '12', 'not null' => 1, 'default' => ''),
      'picture' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'init' => array('type' => 'varchar', 'length' => '64', 'not null' => 0, 'default' => ''),
      'data' => array('type' => 'text', 'not null' => 0)),
    'primary' => array('uid'),
    'unique' => array(
      'users_name_key' => array('name')),
    'index' => array(
      'users_access_idx' => array('access'),
      'users_created_idx' => array('created')),
    );
  $schema['users_roles'] = array(
    'cols' => array(
      'uid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'rid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0)),
    'primary' => array('uid', 'rid'),
    );
  $schema['variable'] = array(
    'cols' => array(
      'name' => array('type' => 'varchar', 'length' => '48', 'not null' => 1, 'default' => ''),
      'value' => array('type' => 'text', 'not null' => 1)),
    'primary' => array('name'),
    );
  $schema['vocabulary'] = array(
    'cols' => array(
      'vid' => array('type' => 'serial', 'not null' => 1),
      'name' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'description' => array('type' => 'text', 'not null' => 0),
      'help' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'relations' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0),
      'hierarchy' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0),
      'multiple' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0),
      'required' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0),
      'tags' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0),
      'module' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'weight' => array('type' => 'int', 'size' => 'small', 'not null' => 1, 'default' => 0)),
    'primary' => array('vid'),
    );
  $schema['vocabulary_node_types'] = array(
    'cols' => array(
      'vid' => array('type' => 'int', 'unsigned' => 1, 'not null' => 1, 'default' => 0),
      'type' => array('type' => 'varchar', 'length' => '32', 'not null' => 1, 'default' => '')),
    'primary' => array('vid', 'type'),
    );
  $schema['watchdog'] = array(
    'cols' => array(
      'wid' => array('type' => 'serial', 'not null' => 1),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0),
      'type' => array('type' => 'varchar', 'length' => '16', 'not null' => 1, 'default' => ''),
      'message' => array('type' => 'text', 'not null' => 1),
      'severity' => array('type' => 'int', 'unsigned' => 1, 'size' => 'small', 'not null' => 1, 'default' => 0),
      'link' => array('type' => 'varchar', 'length' => '255', 'not null' => 1, 'default' => ''),
      'location' => array('type' => 'text', 'not null' => 1, 'default' => ''),
      'referer' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'hostname' => array('type' => 'varchar', 'length' => '128', 'not null' => 1, 'default' => ''),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0)),
    'primary' => array('wid'),
    'index' => array(
      'watchdog_type_idx' => array('type')),
    );
  return _system_schema_merge($schema);
}

?>