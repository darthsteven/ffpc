<?php
class ffpc_plugin_row_podcast extends views_plugin_row_node_rss {

  function render($row) {
    // For the most part, this code is taken from node_feed() in node.module
    global $base_url;

    $item_length = $this->options['item_length'];
    if ($item_length == 'default') {
      $item_length = variable_get('feed_item_length', 'teaser');
    }

    if (empty($this->view->style_plugin->namespaces)) {
      $this->view->style_plugin->namespaces['xmlns:itunes'] = 'http://www.itunes.com/dtds/podcast-1.0.dtd';
    }

    // Load the specified node:
    $item = node_load($row->nid);
    $item->build_mode = NODE_BUILD_RSS;
    $item->link = url("node/$row->nid", array('absolute' => TRUE));

    if ($item_length != 'title') {
      $teaser = ($item_length == 'teaser') ? TRUE : FALSE;

      // Filter and prepare node teaser
      if (node_hook($item, 'view')) {
        $item = node_invoke($item, 'view', $teaser, FALSE);
      }
      else {
        $item = node_prepare($item, $teaser);
      }

      // Allow modules to change $node->teaser before viewing.
      node_invoke_nodeapi($item, 'view', $teaser, FALSE);
    }

    // Allow modules to add additional item fields and/or modify $item
    $extra = node_invoke_nodeapi($item, 'rss item');
    $extra = array_merge($extra,
      array(
        array('key' => 'pubDate', 'value' => gmdate('r', $item->created)),
        // The author should be an email address. Need to add this in.
        //array('key' => 'author', 'value' => $item->name),
      )
    );
    foreach ($extra as $element) {
      if (isset($element['namespace'])) {
        $this->view->style_plugin->namespaces = array_merge($this->view->style_plugin->namespaces, $element['namespace']);
      }
    }

    // Prepare the item description
    switch ($item_length) {
      case 'fulltext':
        $item_text = $item->body;
        break;
      case 'teaser':
        $item_text = $item->teaser;
        if (!empty($item->readmore)) {
          $item_text .= '<p>' . l(t('read more'), 'node/' . $item->nid, array('absolute' => TRUE, 'attributes' => array('target' => '_blank'))) . '</p>';
        }
        break;
      case 'title':
        $item_text = '';
        break;
    }

    $stripped_item_text = strip_tags($item_text);

    if (strlen($stripped_item_text) > 255) {
      $item_subtitle = substr($stripped_item_text, 0, 252) .'...';
    }
    else {
      $item_subtitle = $stripped_item_text;
    }

    if (!getid3_load(TRUE)) {
      return NULL;
    }
    $getid3 = new getID3;
    foreach ( $this->view->field as $id => $field ) {
      if ($field->content_field['widget']['type'] == 'filefield_widget') {
        foreach ( $item->{$field->content_field['field_name']} as $file ) {
          $info = $getid3->analyze($file['filepath']);
          $file_extra = array();
          $file_extra[] = array(
            'key' => 'enclosure',
            'attributes'  =>  array(
              'url' => file_create_url($file['filepath']),
              'length' => $file['filesize'],
              'type' => $file['filemime'],
            ),
          );
          $file_extra[] = array(
            'key' => 'itunes:duration',
            'value' => $info['playtime_string'],
          );
          $file_extra[] = array(
            'key' => 'itunes:author',
            'value' => $info['tags']['id3v2']['artist'][0],
          );

          $file_extra[] = array(
            'key' => 'itunes:subtitle',
            'value' => str_replace('&amp;', '&', $item_subtitle),
          );
          $file_extra[] = array(
            'key' => 'itunes:summary',
            'value' => str_replace('&amp;', '&', $stripped_item_text),
          );
          $file_extra[] = array(
            'key' => 'guid',
            'value' => file_create_url($file['filepath']),
            'attributes' => array('isPermaLink' => 'false'),
          );
          $file_extra = array_merge($extra, $file_extra);
          /*
           * The following function takes title, link, description and then
           * all additional XML elements.  For the title we'll use the node
           * title.  Link serves no real purpose in a podcast.  Description
           * is overridden by the extra "subtitle" tag but we'll keep it for
           * completeness with RSS and use the node teaser.
          */
          $output .= format_rss_item($item->title, $item->link, $item_text, $file_extra );
        }
      }
    }
    return $output;
  }
}