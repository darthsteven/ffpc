<?php
class ffpc_plugin_row_podcast extends views_plugin_row {
	function render( $row ) {
		$output = '';

		$node = node_load( $row->nid );
		if (!getid3_load(TRUE)) {
			return NULL;
		}
		$getid3 = new getID3;

		foreach ( $row as $key => $value ) {
			if ( preg_match('/^(.+)_filepath$/',$key,$matches) ) {
				$prefix = $matches[1];

				$info = $getid3->analyze($row->{$prefix.'_filepath'});
				$extra = array();
				$extra[] = array(
					'key'	=> 'enclosure',
					'attributes'	=>	array(
						'url'		=> url( $row->{$prefix.'_filepath'}, array('absolute'=>TRUE) ),
						'length'	=> $row->{$prefix.'_filesize'},
						'type'		=> $row->{$prefix.'_filemime'},
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
					'value' => url( $row->{$prefix.'_filepath'}, array('absolute'=>TRUE) ),
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

