
Schema module

PREREQUISITES

Drupal 5.0

OVERVIEW

The Schema module provides Drupal with a database schema API, a method
for modules to declare their database tables in structured data rather
than via CREATE TABLE statements, and a means for Drupal to inspect
and reflect on its own database structure.

THIS MODULE IS IN PRE-ALPHA USE AT YOUR OWN RISK RELEASE!  It is
currently for demonstration, testing, and development purposes only.
You've been warned.

INSTALLATION

Install and activate Schema like every other Drupal module.

ADMINISTRATOR USAGE

As an administrator, Schema will show you information about tables
that exist in your database and how they match or do not match up with
the schema information defined by modules.  Visit Administer >> Site
building >> Schema for this information.  Note that these pages are
information-only; there are no actions you can perform from them.

*** IMPORTANT NOTE: Schema expects to find the standard Drupal tables
(declared by system.module) defined as of version 1022 (Drupal 5 head
as of April 14, 2007).  If you are using a different version of Drupal
5, it will report "mismatches" on the Report page.

MODULE DEVELOPER USAGE

Currently, don't use Schema, as everything about it is subject to
change.  

Suppose your module named mymodule wants to use a single simple table
named mytable.  Define hook_schema() in your .module file:

function mymodule_schema() {
  $schema['#version'] = 1;
  $schema['mymodule_mytable] = array(
    'name' => 'mytable',
    'cols' => array(
      'id' => array('type' => 'int', 'not null' => '1'),
      'val => array('type' => 'varchar', 'length' => 255, 'not null' => '1'),
      ),
    'keys' => array('PRIMARY' => array('id')));
  return $schema;
}

Now, in your .install file, use Schema to install and uninstall your
tables:

function schema_install() {
  $my_schema = module_invoke('mymodule, 'schema');
  schema_create_schema($my_schema);
}

function schema_uninstall() {
  $my_schema = module_invoke('mymodule, 'schema');
  schema_drop_schema($my_schema);
}

That's it!  Except, of course, this is pre-alpha code, so that
probably isn't it.
  
TO DO

- Database engines for Postgres and other DBMS's.
- The above .install code actually won't work because mymodule.module
  is not loaded when mymodule_install() is run (or is it?).
- Define join relationships, use them for world domination.

AUTHOR

Barry Jaspan
firstname at lastname dot org
