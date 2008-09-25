<?php

/*
 * Much of the output here is based on the Apple Podcast standard.  Details
 * can be found here:
 * http://www.apple.com/itunes/store/podcaststechspecs.html
*/

function ffpc_views_plugins() {
  return array(
    'module'  => 'ffpc',
    'display' => array(
      'ffpc' => array(
        'title' => t('FileField Podcast'),
        'help' => t('Display the view as a podcast.'),
        'handler' => 'ffpc_plugin_display_ffpc',
        'uses hook menu' => TRUE,
        'use ajax' => FALSE,
        'use pager' => FALSE,
        'parent' => 'feed',
        'accept attachments' => FALSE,
        'admin' => t('FileField Podcast'),
      ),
    ),
    'style'   => array(
      'podcast' => array(
        'title' => t('Podcast'),
        'help' => t('Generates Podcast from a view.'),
        'handler' => 'ffpc_plugin_style_podcast',
        'uses row plugin' => TRUE,
        'uses options' => FALSE,
        'type' => 'ffpc',
      ),
    ),
    'row'   => array(
      'podcast' => array(
        'title' => t('Podcast'),
        'help' => t('Each row is a Podcast element.'),
        'handler' => 'ffpc_plugin_row_podcast',
        'theme' => 'ffpc_view_row_podcast',
        'uses fields' => TRUE,
        'uses options' => FALSE,
        'type' => 'ffpc',
      ),
    ),
  );
}