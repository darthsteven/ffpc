<?php
/**
 * @file
 * Contains the filefield podcast RSS row style plugin.
 */

/**
 * Plugin which performs a node_view on the resulting object
 * and formats it as an iTunes podcast item.
 */
class ffpc_plugin_row_podcast extends views_plugin_row {

  function option_definition() {
    $options = parent::option_definition();

    $options['title_field'] = array('default' => '',);
    $options['author_field'] = array('default' => '',);
    //$options[''] = array('default' => '',);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $field_options = array();
    $fields = $this->display->handler->get_handlers('field');
    foreach ($fields as $id => $handler) {
      $field_options[$id] = $handler->ui_name(FALSE);
    }

    $field_options_optional = array_merge(array('' => t('- None -')), $field_options);

    $form['title_field'] = array(
      '#type' => 'select',
      '#title' => t('Title field'),
      '#options' => $field_options,
      '#default_value' => $this->options['title_field'],
      '#title' => t('This will be used as the title of podcast epsodes.'),
      '#required' => TRUE,
    );

    $form['author_field'] = array(
      '#type' => 'select',
      '#title' => t('Author field'),
      '#options' => $field_options_optional,
      '#default_value' => $this->options['author_field'],
      '#title' => t('This will be used as the author of podcast epsodes.'),
    );
  }

  function render($row) {
    // For the most part, this code is taken from node_feed() in node.module
    global $base_url;

    dsm($this->view->row_index);
    dsm($this->view->style_plugin->get_field($this->view->row_index, $this->options['author_field']));

    $rss_namespaces = array('xmlns:dc' => 'http://purl.org/dc/elements/1.1/');
    $rss_elements = array(
      //array('key' => 'pubDate', 'value' => gmdate('r', $node->created)),
      //array('key' => 'dc:creator', 'value' => $node->name),
      //array('key' => 'guid', 'value' => $node->nid . ' at ' . $base_url, 'attributes' => array('isPermaLink' => 'false'))
    );

    $this->view->style_plugin->namespaces = array_merge($this->view->style_plugin->namespaces, $rss_namespaces);

    $item = new stdClass;
    //$item->description = $item_text;

    $title_field_obj = $this->view->display_handler->get_handler('field', $this->options['title_field']);
    $title_field = $title_field_obj->field_alias;
    $item->title = $row->$title_field;

    if (!empty($this->options['author_field'])) {
      $author_field_obj = $this->view->display_handler->get_handler('field', $this->options['author_field']);
      //dsm($author_field_obj->render();
      $author_field = $author_field_obj->field_alias;
      $rss_elements[] = array('key' => 'author', 'value' => $author_field);
    }


    //$item->link = url("node/$node->nid", array('absolute' => TRUE));
    $item->elements = $rss_elements;
    //$item->nid = $node->nid;
    //if (isset($node->readmore)) {
    //  $item->readmore = $node->readmore;
    //}

    return theme($this->theme_functions(),
      array(
        'view' => $this->view,
        'options' => $this->options,
        'row' => $item
      ));
  }

  function old_render($row) {
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
        node_object_prepare($item, $teaser);
      }
      // Allow modules to change $node->teaser before viewing.
      module_invoke_all('node_view', $item, $teaser, FALSE);
    }

    // Allow modules to add additional item fields and/or modify $item
    $extra = module_invoke_all('node_rss_item', $item);
    $extra = array_merge($extra,
      array(
        array('key' => 'pubDate', 'value' => $item->field_time['und'][0]['value']),
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
        $item_text = $item->field_text['und'][0]['safe_value'];;
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
      if ($field->field_info['type'] == 'file') {
        foreach ( $item->{$field->field_info['field_name']} as $f ) {
          $file = $f[0];
	  $info = $getid3->analyze(drupal_realpath($file['uri']));
          $file_extra = array();
          $file_extra[] = array(
            'key' => 'enclosure',
            'attributes'  =>  array(
              'url' => file_create_url($file['uri']),
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
          /*$file_extra[] = array(
            'key' => 'guid',
            'value' => file_create_url($file['filepath']),
            'attributes' => array('isPermaLink' => 'false'),
          );*/
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
