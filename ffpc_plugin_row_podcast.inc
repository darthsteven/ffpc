<?php
class ffpc_plugin_row_podcast extends views_plugin_row {
	function render( $row ) {
		//watchdog('ffpc','ffpc_plugin_row_podcast::render called for node ' . $row->nid);
		$output = '';

		$node = node_load( $row->nid );
		if (!getid3_load(TRUE)) {
			return NULL;
		}
		$getid3 = new getID3;

		// these pairs are no longer valid.... $field = a giant recursed object that doesn't contains $field->content_field
		//                                     $id    = the fieldname from the field_audio object
		//foreach ( $this->view->field as $id => $field ) {

			// this dies now. eek. New views don't $field->content_field anymore :(
			//foreach ( $node->{$field->content_field['field_name']} as $file ) {

			foreach ( $node->field_audio as $file ) {
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
		//}
		return $output;
	}
}

