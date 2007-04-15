<?php

function system_schema()
{
  $schema['#version'] = '1022';

  $schema['access'] = array(
    'cols' => array(
      'aid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '11'),
      'mask' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'type' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'status' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4')),
    'keys' => array(
      'PRIMARY' => array('aid')),
    );
  $schema['authmap'] = array(
    'cols' => array(
      'aid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'authname' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'module' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128')),
    'keys' => array(
      'authname' => array('authname', 'unique'),
      'PRIMARY' => array('aid')),
    );
  $schema['blocks'] = array(
    'cols' => array(
      'module' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '64'),
      'delta' => array('type' => 'varchar', 'not null' => 1, 'default' => '0', 'length' => '32'),
      'theme' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'status' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'weight' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'region' => array('type' => 'varchar', 'not null' => 1, 'default' => 'left', 'length' => '64'),
      'custom' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'throttle' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'visibility' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'pages' => array('type' => 'text', 'not null' => 1),
      'title' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '64')),
    );
  $schema['blocks_roles'] = array(
    'cols' => array(
      'module' => array('type' => 'varchar', 'not null' => 1, 'length' => '64'),
      'delta' => array('type' => 'varchar', 'not null' => 1, 'length' => '32'),
      'rid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'PRIMARY' => array('module, delta, rid')),
    );
  $schema['boxes'] = array(
    'cols' => array(
      'bid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '11'),
      'body' => array('type' => 'longtext', 'not null' => 0),
      'info' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'format' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11')),
    'keys' => array(
      'info' => array('info', 'unique'),
      'PRIMARY' => array('bid')),
    );
  $schema['cache'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'data' => array('type' => 'longblob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'keys' => array(
      'expire' => array('expire'),
      'PRIMARY' => array('cid')),
    );
  $schema['cache_content'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'data' => array('type' => 'longblob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'keys' => array(
      'expire' => array('expire'),
      'PRIMARY' => array('cid')),
    );
  $schema['cache_filter'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'data' => array('type' => 'longblob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'keys' => array(
      'expire' => array('expire'),
      'PRIMARY' => array('cid')),
    );
  $schema['cache_menu'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'data' => array('type' => 'longblob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'keys' => array(
      'expire' => array('expire'),
      'PRIMARY' => array('cid')),
    );
  $schema['cache_page'] = array(
    'cols' => array(
      'cid' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'data' => array('type' => 'longblob', 'not null' => 0),
      'expire' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'headers' => array('type' => 'text', 'not null' => 0)),
    'keys' => array(
      'expire' => array('expire'),
      'PRIMARY' => array('cid')),
    );
  $schema['comments'] = array(
    'cols' => array(
      'cid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '11'),
      'pid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'nid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'subject' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '64'),
      'comment' => array('type' => 'longtext', 'not null' => 1),
      'hostname' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'score' => array('type' => 'mediumint', 'not null' => 1, 'default' => 0, 'disp_width' => '9'),
      'status' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1),
      'format' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'thread' => array('type' => 'varchar', 'not null' => 1, 'length' => '255'),
      'users' => array('type' => 'longtext', 'not null' => 0),
      'name' => array('type' => 'varchar', 'not null' => 0, 'length' => '60'),
      'mail' => array('type' => 'varchar', 'not null' => 0, 'length' => '64'),
      'homepage' => array('type' => 'varchar', 'not null' => 0, 'length' => '255')),
    'keys' => array(
      'lid' => array('nid'),
      'PRIMARY' => array('cid')),
    );
  $schema['files'] = array(
    'cols' => array(
      'fid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'nid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'filename' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'filepath' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'filemime' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'filesize' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'nid' => array('nid'),
      'PRIMARY' => array('fid')),
    );
  $schema['file_revisions'] = array(
    'cols' => array(
      'fid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'vid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'description' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'list' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1)),
    'keys' => array(
      'PRIMARY' => array('fid, vid'),
      'vid' => array('vid')),
    );
  $schema['filters'] = array(
    'cols' => array(
      'format' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'module' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '64'),
      'delta' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'weight' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4')),
    'keys' => array(
      'weight' => array('weight')),
    );
  $schema['filter_formats'] = array(
    'cols' => array(
      'format' => array('type' => 'int', 'not null' => 1, 'disp_width' => '11'),
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'roles' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'cache' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4')),
    'keys' => array(
      'name' => array('name', 'unique'),
      'PRIMARY' => array('format')),
    );
  $schema['flood'] = array(
    'cols' => array(
      'event' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '64'),
      'hostname' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11')),
    );
  $schema['history'] = array(
    'cols' => array(
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'nid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11')),
    'keys' => array(
      'PRIMARY' => array('uid, nid')),
    );
  $schema['menu'] = array(
    'cols' => array(
      'mid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'pid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'path' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'title' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'description' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'weight' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'type' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'PRIMARY' => array('mid')),
    );
  $schema['node'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'vid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'type' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '32'),
      'title' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'status' => array('type' => 'int', 'not null' => 1, 'default' => 1, 'disp_width' => '11'),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'changed' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'comment' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'promote' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'moderate' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'sticky' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11')),
    'keys' => array(
      'nid' => array('nid'),
      'node_changed' => array('changed'),
      'node_created' => array('created'),
      'node_moderate' => array('moderate'),
      'node_promote_status' => array('promote, status'),
      'node_status_type' => array('status, type, nid'),
      'node_title_type' => array('title, type'),
      'node_type' => array('type'),
      'PRIMARY' => array('nid, vid'),
      'status' => array('status'),
      'uid' => array('uid'),
      'vid' => array('vid', 'unique')),
    );
  $schema['node_access'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'gid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'realm' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'grant_view' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1),
      'grant_update' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1),
      'grant_delete' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1)),
    'keys' => array(
      'PRIMARY' => array('nid, gid, realm')),
    );
  $schema['node_comment_statistics'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'last_comment_timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'last_comment_name' => array('type' => 'varchar', 'not null' => 0, 'length' => '60'),
      'last_comment_uid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'comment_count' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'node_comment_timestamp' => array('last_comment_timestamp'),
      'PRIMARY' => array('nid')),
    );
  $schema['node_counter'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'totalcount' => array('type' => 'bigint', 'not null' => 1, 'default' => 0, 'disp_width' => '20', 'unsigned' => 1),
      'daycount' => array('type' => 'mediumint', 'not null' => 1, 'default' => 0, 'disp_width' => '8', 'unsigned' => 1),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'daycount' => array('daycount'),
      'PRIMARY' => array('nid'),
      'timestamp' => array('timestamp'),
      'totalcount' => array('totalcount')),
    );
  $schema['node_field'] = array(
    'cols' => array(
      'field_name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '32'),
      'type' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '127'),
      'global_settings' => array('type' => 'mediumtext', 'not null' => 1),
      'required' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'multiple' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'db_storage' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11')),
    'keys' => array(
      'PRIMARY' => array('field_name')),
    );
  $schema['node_field_instance'] = array(
    'cols' => array(
      'field_name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '32'),
      'type_name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '32'),
      'weight' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'label' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'widget_type' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '32'),
      'widget_settings' => array('type' => 'mediumtext', 'not null' => 1),
      'display_settings' => array('type' => 'mediumtext', 'not null' => 1),
      'description' => array('type' => 'mediumtext', 'not null' => 1)),
    'keys' => array(
      'PRIMARY' => array('field_name, type_name')),
    );
  $schema['node_revisions'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'vid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'title' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'body' => array('type' => 'longtext', 'not null' => 1),
      'teaser' => array('type' => 'longtext', 'not null' => 1),
      'log' => array('type' => 'longtext', 'not null' => 1),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'format' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11')),
    'keys' => array(
      'nid' => array('nid'),
      'PRIMARY' => array('vid'),
      'uid' => array('uid')),
    );
  $schema['node_type'] = array(
    'cols' => array(
      'type' => array('type' => 'varchar', 'not null' => 1, 'length' => '32'),
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'module' => array('type' => 'varchar', 'not null' => 1, 'length' => '255'),
      'description' => array('type' => 'mediumtext', 'not null' => 1),
      'help' => array('type' => 'mediumtext', 'not null' => 1),
      'has_title' => array('type' => 'tinyint', 'not null' => 1, 'disp_width' => '3', 'unsigned' => 1),
      'title_label' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'has_body' => array('type' => 'tinyint', 'not null' => 1, 'disp_width' => '3', 'unsigned' => 1),
      'body_label' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'min_word_count' => array('type' => 'smallint', 'not null' => 1, 'disp_width' => '5', 'unsigned' => 1),
      'custom' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'modified' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'locked' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'orig_type' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255')),
    'keys' => array(
      'PRIMARY' => array('type')),
    );
  $schema['permission'] = array(
    'cols' => array(
      'rid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'perm' => array('type' => 'longtext', 'not null' => 0),
      'tid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'rid' => array('rid')),
    );
  $schema['role'] = array(
    'cols' => array(
      'rid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '64')),
    'keys' => array(
      'name' => array('name', 'unique'),
      'PRIMARY' => array('rid')),
    );
  $schema['schema_modules'] = array(
    'cols' => array(
      'mid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '11'),
      'name' => array('type' => 'varchar', 'not null' => 1, 'length' => '255')),
    'keys' => array(
      'PRIMARY' => array('mid')),
    );
  $schema['schema_tables'] = array(
    'cols' => array(
      'tid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '11'),
      'name' => array('type' => 'varchar', 'not null' => 1, 'length' => '255'),
      'mid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '11')),
    'keys' => array(
      'mid' => array('mid'),
      'PRIMARY' => array('tid')),
    );
  $schema['sequences'] = array(
    'cols' => array(
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'id' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'PRIMARY' => array('name')),
    );
  $schema['sessions'] = array(
    'cols' => array(
      'uid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'sid' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '64'),
      'hostname' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'cache' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'session' => array('type' => 'longtext', 'not null' => 0)),
    'keys' => array(
      'PRIMARY' => array('sid'),
      'timestamp' => array('timestamp'),
      'uid' => array('uid')),
    );
  $schema['system'] = array(
    'cols' => array(
      'filename' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'type' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'description' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'status' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'throttle' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'bootstrap' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'schema_version' => array('type' => 'smallint', 'not null' => 1, 'default' => -1, 'disp_width' => '6'),
      'weight' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11')),
    'keys' => array(
      'PRIMARY' => array('filename'),
      'weight' => array('weight')),
    );
  $schema['term_data'] = array(
    'cols' => array(
      'tid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'vid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'description' => array('type' => 'longtext', 'not null' => 0),
      'weight' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4')),
    'keys' => array(
      'PRIMARY' => array('tid'),
      'vid' => array('vid')),
    );
  $schema['term_hierarchy'] = array(
    'cols' => array(
      'tid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'parent' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'parent' => array('parent'),
      'PRIMARY' => array('tid, parent'),
      'tid' => array('tid')),
    );
  $schema['term_node'] = array(
    'cols' => array(
      'nid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'tid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'nid' => array('nid'),
      'PRIMARY' => array('tid, nid'),
      'tid' => array('tid')),
    );
  $schema['term_relation'] = array(
    'cols' => array(
      'tid1' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'tid2' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'tid1' => array('tid1'),
      'tid2' => array('tid2')),
    );
  $schema['term_synonym'] = array(
    'cols' => array(
      'tid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255')),
    'keys' => array(
      'name' => array('name'),
      'tid' => array('tid')),
    );
  $schema['url_alias'] = array(
    'cols' => array(
      'pid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'src' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'dst' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128')),
    'keys' => array(
      'dst' => array('dst', 'unique'),
      'PRIMARY' => array('pid'),
      'src' => array('src')),
    );
  $schema['users'] = array(
    'cols' => array(
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '60'),
      'pass' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '32'),
      'mail' => array('type' => 'varchar', 'not null' => 0, 'default' => '', 'length' => '64'),
      'mode' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'sort' => array('type' => 'tinyint', 'not null' => 0, 'default' => 0, 'disp_width' => '4'),
      'threshold' => array('type' => 'tinyint', 'not null' => 0, 'default' => 0, 'disp_width' => '4'),
      'theme' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'signature' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'created' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'access' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'login' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'status' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4'),
      'timezone' => array('type' => 'varchar', 'not null' => 0, 'length' => '8'),
      'language' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '12'),
      'picture' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'init' => array('type' => 'varchar', 'not null' => 0, 'default' => '', 'length' => '64'),
      'data' => array('type' => 'longtext', 'not null' => 0)),
    'keys' => array(
      'access' => array('access'),
      'created' => array('created'),
      'name' => array('name', 'unique'),
      'PRIMARY' => array('uid')),
    );
  $schema['users_roles'] = array(
    'cols' => array(
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'rid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1)),
    'keys' => array(
      'PRIMARY' => array('uid, rid')),
    );
  $schema['variable'] = array(
    'cols' => array(
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '48'),
      'value' => array('type' => 'longtext', 'not null' => 1)),
    'keys' => array(
      'PRIMARY' => array('name')),
    );
  $schema['vocabulary'] = array(
    'cols' => array(
      'vid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '10', 'unsigned' => 1),
      'name' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'description' => array('type' => 'longtext', 'not null' => 0),
      'help' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'relations' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1),
      'hierarchy' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1),
      'multiple' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1),
      'required' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1),
      'tags' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1),
      'module' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'weight' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '4')),
    'keys' => array(
      'PRIMARY' => array('vid')),
    );
  $schema['vocabulary_node_types'] = array(
    'cols' => array(
      'vid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '10', 'unsigned' => 1),
      'type' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '32')),
    'keys' => array(
      'PRIMARY' => array('vid, type')),
    );
  $schema['watchdog'] = array(
    'cols' => array(
      'wid' => array('type' => 'int', 'not null' => 1, 'disp_width' => '11'),
      'uid' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11'),
      'type' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '16'),
      'message' => array('type' => 'longtext', 'not null' => 1),
      'severity' => array('type' => 'tinyint', 'not null' => 1, 'default' => 0, 'disp_width' => '3', 'unsigned' => 1),
      'link' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '255'),
      'location' => array('type' => 'text', 'not null' => 1),
      'referer' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'hostname' => array('type' => 'varchar', 'not null' => 1, 'default' => '', 'length' => '128'),
      'timestamp' => array('type' => 'int', 'not null' => 1, 'default' => 0, 'disp_width' => '11')),
    'keys' => array(
      'PRIMARY' => array('wid'),
      'type' => array('type')),
    );
  return $schema;
}

?>