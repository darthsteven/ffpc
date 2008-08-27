<?php

/*
 * Much of the output here is based on the Apple Podcast standard.  Details
 * can be found here:
 * http://www.apple.com/itunes/store/podcaststechspecs.html
*/

function ffpc_views_plugins() {
	return array(
		'module'	=> 'ffpc',
		'display'	=> array(
			'ffpc' => array(
				'title' => t('FileField Podcast'),
				'help' => t('Display the view as a podcast.'),
				'handler' => 'ffpc_plugin_display_ffpc',
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
				'theme' => 'ffpc_view_row_podcast',
				'uses fields' => TRUE,
				'uses options' => FALSE,
				'type' => 'ffpc',
			),
		),
	);
}

/**
 * The plugin that handles a podcast.
 */
class ffpc_plugin_display_ffpc extends views_plugin_display_feed {
	function get_style_type() { return 'ffpc'; }

	function option_definition() {
		$options = parent::option_definition();

		$options['style_plugin']['default'] = 'podcast';
		$options['row_plugin']['default'] = 'podcast';

		return $options;
	}

	function options_summary(&$categories, &$options) {
		parent::options_summary($categories, $options);

		// disable options that aren't applicable
		unset($options['footer']);
		unset($options['empty']);
		unset($options['analyze-theme']);
	}
}

/**
 * Default style plugin to render a Podcast
 */
class ffpc_plugin_style_podcast extends views_plugin_style {
	function render() {
		global $base_url, $language;
		if (empty($this->row_plugin)) {
			vpr('views_plugin_style_default: Missing row plugin');
			return;
		}
		$rows = '';

		foreach ($this->view->result as $row) {
			$rows .= $this->row_plugin->render($row);
		}

		$channel_defaults = array(
			'version'		 => '2.0',
			'title'			 => variable_get('site_name', 'Drupal'),
			'link'				=> $base_url,
			'description' => variable_get('site_mission', ''),
			'language'		=> $language->language
		);
		$channel = array();
		$channel = array_merge($channel_defaults, $channel);

		$args = array(
			'itunes:summary' => $this->view->display_handler->render_header(),
		);
	
		$namespaces = array('xmlns:itunes' => 'http://www.itunes.com/dtds/podcast-1.0.dtd');
		$output = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
		$output .= "<rss version=\"". $channel["version"] ."\" ". drupal_attributes($namespaces) .">\n";
		$output .= format_rss_channel(
			$this->view->get_title(),
			$base_url,
			$this->view->display_handler->render_header(),
			$rows,
			$channel['language'],
			$args
		);
		$output .= "</rss>\n";

		drupal_set_header('Content-Type: application/rss+xml; charset=utf-8');
		print $output;
	}
}


class ffpc_plugin_row_podcast extends views_plugin_row {
	function render( $row ) {
		$output = '';

		$node = node_load( $row->nid );
		if (!getid3_load(TRUE)) {
			return NULL;
		}
		$getid3 = new getID3;
		foreach ( $this->view->field as $id => $field ) {
			foreach ( $node->{$field->content_field['field_name']} as $file ) {
				$info = $getid3->analyze($file['filepath']);
				$extra = array();
				$extra[] = array(
					'key'	=> 'enclosure',
					'attributes'	=>	array(
						'url'		=> url( $file['filepath'], array('absolute'=>TRUE) ),
						'length'	=> $file['filesize'],
						'type'		=> $file['filemime'],
					),
				);
				$extra[] = array(
					'key'	=> 'itunes:duration',
					'value'	=> $info['playtime_string'],
				);
				$extra[] = array(
					'key'	=> 'itunes:author',
					'value'	=> $info['tags']['id3v2']['artist'][0],
				);
				$extra[] = array(
					'key'	=> 'itunes:subtitle',
					'value'	=> $node->teaser,
				);
				$extra[] = array(
					'key'	=> 'itunes:summary',
					'value'	=> $node->body,
				);
				$extra[] = array('key' => 'pubDate', 'value' => gmdate('r', $node->created));
				$extra[] = array(
					'key' => 'guid',
					'value' => url( $file['filepath'], array('absolute'=>TRUE) ),
				);
				/*
				 * The following function takes title, link, description and then
				 * all additional XML elements.  For the title we'll use the node
				 * title.  Link serves no real purpose in a podcast.  Description
				 * is overridden by the extra "subtitle" tag but we'll keep it for
				 * completeness with RSS and use the node teaser.
				*/
				$output .= format_rss_item( $node->title, NULL, $node->teaser, $extra );
			}
		}
		return $output;
	}
}
