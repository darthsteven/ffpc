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
    // Get all the enclosure field types that we handle.
    $enclosure_field_types = array_keys(ffpc_get_enclosure_field_types());
    foreach ($fields as $id => $handler) {
      $field_options[$id] = $handler->ui_name(FALSE);
      if (isset($handler->field_info['type'])) {
        if (in_array($handler->field_info['type'], $enclosure_field_types)) {
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

    // Now handle the enclosure.
    $enclosure_field_types = ffpc_get_enclosure_field_types();
    $field_handler = $this->view->display_handler->get_handler('field', $this->options['enclosure_field']);
    if (isset($field_handler->field_info['type'])) {
      $render_callback = $enclosure_field_types[$field_handler->field_info['type']]['render callback'];
      if (function_exists($render_callback)) {
        $view_values = $this->view->result[$this->view->row_index];
        $render_callback($rss_elements, $view_values, $field_handler);
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
}
