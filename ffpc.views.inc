<?php

/*
 * Much of the output here is based on the Apple Podcast standard.  Details
 * can be found here:
 * http://www.apple.com/itunes/store/podcaststechspecs.html
*/

function ffpc_views_handlers() {
	return array(
		'info' => array(
			'path' => drupal_get_path('module','ffpc')
		),
		'handlers' => array(
			'ffpc_plugin_display_ffpc' => array(
				'parent' => 'views_plugin_display_feed'
			),
			'ffpc_plugin_style_podcast' => array(
				'parent' => 'views_plugin_style'
			),
			'ffpc_plugin_row_podcast' => array(
				'parent' => 'views_plugin_row'
			),
		)
	);
}

function ffpc_views_plugins() {
	$data = array(
		'module'	=> 'ffpc',
		'display'	=> array(
			'ffpc' => array(
				'title' => t('FileField Podcast'),
				'help' => t('Display the view as a podcast.'),
				'handler' => 'ffpc_plugin_display_ffpc',
				'parent' => 'feed',
				'path' => drupal_get_path('module','ffpc'),
				'uses hook menu' => TRUE,
				'use ajax' => FALSE,
				'use pager' => FALSE,
				'accept attachments' => FALSE,
				'admin' => t('FileField Podcast'),
			),
		),
		'style'		=> array(
			'podcast'	=> array(
				'title' => t('Podcast'),
				'help' => t('Generates Podcast from a view.'),
				'handler' => 'ffpc_plugin_style_podcast',
				'path' => drupal_get_path('module','ffpc'),
				'uses row plugin' => TRUE,
				'uses options' => FALSE,
				'type' => 'ffpc',
			),
		),
		'row'		=> array(
			'podcast'	=> array(
				'title' => t('Podcast'),
				'help' => t('Each row is a Podcast element.'),
				'handler' => 'ffpc_plugin_row_podcast',
				'path' => drupal_get_path('module','ffpc'),
				'theme' => 'ffpc_view_row_podcast',
				'uses fields' => TRUE,
				'uses options' => FALSE,
				'type' => 'ffpc',
			),
		),
	);
	return $data;
}
