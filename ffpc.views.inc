<?php
/**
 * @file
 * Views plugins for FileField Podcaster.
 *
 * Much of the output here is based on the Apple Podcast standard.
 * @see http://www.apple.com/itunes/store/podcaststechspecs.html
 * 
 * Note: We need a custom style plugin because the default RSS plugin does not
 * permit additional elements to be added to the feed.
 */

/**
 * Implements hook_views_plugins().
 */
function ffpc_views_plugins() {
  return array(
    'module' => 'ffpc',
    'style' => array(
      'podcast' => array(
        'title' => t('Podcast Feed'),
        'help' => t('Generates Podcast Feed from a view.'),
        'handler' => 'ffpc_plugin_style_podcast',
        'theme' => 'ffpc_view_podcast_feed',
        'uses row plugin' => TRUE,
        'uses options' => TRUE,
        'uses fields' => TRUE,
        'uses grouping' => FALSE,
	'even empty' => TRUE,
        'type' => 'feed',
        'path' => drupal_get_path('module', 'ffpc'),
      ),
    ),
    'row' => array(
      'podcast_node' => array(
        'title' => t('Podcast Episode'),
        'help' => t('Display the node as a Podcast Episode.'),
        'handler' => 'ffpc_plugin_row_podcast',
        'uses fields' => TRUE,
        'type' => 'feed',
        'base' => array('node'), // only works with 'node' as base.
        'path' => drupal_get_path('module', 'ffpc'),
        'uses options' => TRUE,
      ),
    ),
  );
}
