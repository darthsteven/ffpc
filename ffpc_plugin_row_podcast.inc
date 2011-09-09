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

    $options['title_field'] = array(
      'default' => '',
    );
    $options['enclosure_field'] = array(
      'default' => '',
    );
    $options['author_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'author',
        'title' => t('Author field'),
        'description' => t('This will be used as the author of podcast episodes.'),
        ),
    );
    $options['subtitle_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'itunes:subtitle',
        'title' => t('Subtitle field'),
        'description' => t('The contents of this tag are shown in the Description column in iTunes. The subtitle displays best if it is only a few words long.'),
      ),
    );
    $options['summary_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'description',
        'title' => t('Summary field'),
        'description' => t('The contents of this tag are shown in a separate window that appears when the "circled i" in the Description column is clicked. It also appears on the iTunes page for your podcast. This field can be up to 4000 characters.'),
      ),
    );
    $options['keywords_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'itunes:keywords',
        'title' => t('Keywords field'),
        'description' => t('This tag allows users to search on a maximum of 12 text keywords. Use commas to separate keywords.'),
      ),
    );
    /**
     * Todo:
     * - pubDate
     * - itunes:block
     * - itunes:image
     * - itunes:duration
     * - itunes:explicit
     * - itunes:isClosedCaptioned
     * - itunes:order
     * - enclosure
     * - guid
     */




    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $field_options = array();
    $fields = $this->display->handler->get_handlers('field');
    $file_fields = array();
    // Get all the field types that we handle.
    // TODO: Factor this out somewhere.
    $file_field_types = module_invoke_all('ffpc_file_field_types');
    drupal_alter('ffpc_file_field_types', $file_field_types);
    $file_field_field_types = array_keys($file_field_types);
    foreach ($fields as $id => $handler) {
      $field_options[$id] = $handler->ui_name(FALSE);
      if (isset($handler->field_info['type'])) {
        if (in_array($handler->field_info['type'], $file_field_field_types)) {
          $file_fields[$id] = $handler->ui_name(FALSE);
        }
      }
    }

    $field_options_optional = array_merge(array('' => t('- None -')), $field_options);

    $form['enclosure_field'] = array(
      '#type' => 'select',
      '#title' => t('Enclosure field'),
      '#options' => $file_fields,
      '#default_value' => $this->options['enclosure_field'],
      '#description' => t('This will be used as the file for the podcast episode.'),
      '#required' => TRUE,
    );

    $form['title_field'] = array(
      '#type' => 'select',
      '#title' => t('Title field'),
      '#options' => $field_options,
      '#default_value' => $this->options['title_field'],
      '#description' => t('This will be used as the title of podcast episodes.'),
      '#required' => TRUE,
    );

	foreach ($this->option_definition() as $option => $options) {
	  if (isset($options['ffpc'])) {
		$form[$option] = array(
		  '#type' => 'select',
		  '#title' => $options['ffpc']['title'],
		  '#options' => $field_options_optional,
		  '#default_value' => $this->options[$option],
		  '#description' => isset($options['ffpc']['description']) ? $options['ffpc']['description'] : '',
		);
		if (isset($options['ffpc']['required']) && $options['ffpc']['required']) {
		  $form[$option]['#options'] = $field_options;
		}
	  }
	}
  }

  function render_field($field, $from_options = TRUE, $strip_tags = TRUE) {
	$render = '';
	if ($from_options) {
	  if (isset($this->options[$field])) {
		$field = $this->options[$field];
	  }
	  else {
		$field = '';
	  }
	}
	if (!empty($field)) {
	  $idx = $this->view->row_index;
	  $render = $this->view->style_plugin->get_field($this->view->row_index, $field);
	  $this->view->row_index = $idx;
	}

	if (!empty($render) && (strpos($render, '<') !== FALSE)) {
	  $render = strip_tags($render);
	  if (!empty($render)) {
		$render = trim($render);
	  }
	}
	return $render;
  }

  function render($row) {
    // For the most part, this code is taken from node_feed() in node.module
    global $base_url;

    $rss_namespaces = array(
	  'xmlns:itunes' => 'http://www.itunes.com/dtds/podcast-1.0.dtd',
	);
    $rss_elements = array(
      //array('key' => 'pubDate', 'value' => gmdate('r', $node->created)),
      //array('key' => 'guid', 'value' => $node->nid . ' at ' . $base_url, 'attributes' => array('isPermaLink' => 'false'))
    );

    $this->view->style_plugin->namespaces = array_merge($this->view->style_plugin->namespaces, $rss_namespaces);

    $item = new stdClass;

    // Add fields to the item that are special
    $item->title = $this->render_field('title_field');

    // Add all the other elements, defined in options.
    foreach ($this->option_definition() as $option => $options) {
      if (isset($options['ffpc']['xml_element'])) {
        if ($value = $this->render_field($option)) {
          $rss_elements[] = array('key' => $options['ffpc']['xml_element'], 'value' => $value);
        }
      }
    }

    // Get all the field types that we handle.
    // TODO: Factor this out somewhere.
    $file_field_types = module_invoke_all('ffpc_file_field_types');
    drupal_alter('ffpc_file_field_types', $file_field_types);
    $file_field = $this->view->display_handler->get_handler('field', $this->options['enclosure_field']);
    if (isset($file_field->field_info['type'])) {
      $render_callback = $file_field_types[$file_field->field_info['type']]['render callback'];
      if (function_exists($render_callback)) {
        $file_value = $file_field->get_value($this->view->result[$this->view->row_index]);
        $render_callback($rss_elements, $file_value, $file_field);
      }
    }

    $item->elements = $rss_elements;

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
