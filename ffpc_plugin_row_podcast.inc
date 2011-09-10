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
    $options['guid_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'guid',
        'title' => t('GUID field'),
        'description' => t('This will be used as the GUID of podcast episodes.'),
      ),
    );
    $options['guid_perma'] = array(
      'default' => FALSE,
    );
    $options['pubdate_field'] = array(
      'default' => '',
    );
    $options['link_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'link',
        'title' => t('Link field'),
        'description' => t('This will be used as a Link for podcast episodes.'),
        'on_object' => TRUE,
      ),
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
        'title' => t('iTunes Subtitle field'),
        'description' => t('The contents of this tag are shown in the Description column in iTunes. The subtitle displays best if it is only a few words long.'),
      ),
    );
    $options['summary_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'description',
        'title' => t('Summary field'),
        'description' => t('The contents of this tag are shown in a separate window that appears when the "circled i" in the Description column is clicked. It also appears on the iTunes page for your podcast. This field can be up to 4000 characters.'),
        'on_object' => TRUE,
      ),
    );
    $options['keywords_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'itunes:keywords',
        'title' => t('iTunes Keywords field'),
        'description' => t('This tag allows users to search on a maximum of 12 text keywords. Use commas to separate keywords.'),
      ),
    );
    $options['itunes_block_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'itunes:block',
        'title' => t('iTunes block field'),
        'description' => t('Use this to prevent an episode from appearing in the iTunes Podcast directory. For example, you may want a specific episode blocked from iTunes if its content might cause the feed to be removed from iTunes. If this tag is present and set to "yes" (case insensitive), that means to block the the episode.'),
      ),
    );
    $options['itunes_duration_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'itunes:duration',
        'title' => t('iTunes duration field'),
        'description' => t('The content of this tag is shown in the Time column in iTunes. The tag can be formatted HH:MM:SS, H:MM:SS, MM:SS, or M:SS (H = hours, M = minutes, S = seconds). If an integer is provided (no colon present), the value is assumed to be in seconds. If one colon is present, the number to the left is assumed to be minutes, and the number to the right is assumed to be seconds. If more than two colons are present, the numbers furthest to the right are ignored.'),
      ),
    );
    $options['itunes_explicit_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'itunes:explicit',
        'title' => t('iTunes duration field'),
        'description' => t('This tag should be used to indicate whether or not your podcast contains explicit material. The three values for this tag are "yes", "no", and "clean".'),
      ),
    );
    $options['itunes_explicit_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'itunes:isClosedCaptioned',
        'title' => t('iTunes Closed Captioned field'),
        'description' => t('This tag should be used if your episode includes a video with embedded closed captioning support. The two values for this tag are "yes" and "no".'),
      ),
    );
    $options['itunes_order_field'] = array(
      'default' => '',
      'ffpc' => array(
        'xml_element' => 'itunes:order',
        'title' => t('iTunes Order field'),
        'description' => t('This tag can be used to override the default ordering of episodes on the store.'),
      ),
    );

    return $options;
  }

  protected function compute_field_options($type, $fields) {
    $options = array();
    $ffpc_handlers = ffpc_get_handled_field_types($type);
    foreach ($fields as $id => &$handler) {
      foreach ($ffpc_handlers as $ffpc_id => $ffpc_handler) {
        if (isset($ffpc_handler['identification callback']) && function_exists($ffpc_handler['identification callback'])) {
          $function = $ffpc_handler['identification callback'];
          if ($function($handler)) {
            $options[$id . ':' . $ffpc_id] = $handler->ui_name(FALSE);
            // Once it's been identified, we're done.
            break;
          }
        }
      }
    }

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $field_options = array();
    $fields = $this->display->handler->get_handlers('field');
    $file_fields = array();

    foreach ($fields as $id => $handler) {
      $field_options[$id] = $handler->ui_name(FALSE);
    }
    $field_options_optional = array_merge(array('' => t('- None -')), $field_options);

    // Get all the enclosure field types that we handle.
    $enclosure_field_types = $this->compute_field_options('file', $fields);
    $date_field_types = $this->compute_field_options('date', $fields);

    $form['_description'] = array(
      '#markup' => t('These options allow you to use normal views fields as the source of the fields in the podcast feed. You can use the rewriting options of the views fields to alter the values that will appear in the feed.'),
    );

    $form['enclosure_field'] = array(
      '#type' => 'select',
      '#title' => t('Enclosure field'),
      '#options' => $enclosure_field_types,
      '#default_value' => $this->options['enclosure_field'],
      '#description' => t('This will be used as the file for the podcast episode.'),
      '#required' => TRUE,
    );
    if (empty($enclosure_field_types)) {
      $form['enclosure_field']['#description'] .= '<br />' . t('<strong>Try adding a file field to your views fields to enable an option here.</strong>');
    }

    $form['title_field'] = array(
      '#type' => 'select',
      '#title' => t('Title field'),
      '#options' => $field_options,
      '#default_value' => $this->options['title_field'],
      '#description' => t('This will be used as the title of podcast episodes.'),
      '#required' => TRUE,
    );


    $form['pubdate_field'] = array(
      '#type' => 'select',
      '#title' => t('Publication date field'),
      '#options' => $date_field_types,
      '#default_value' => $this->options['pubdate_field'],
      '#description' => t('This specifies the date and time when an episode was released.'),
    );
    if (empty($date_field_types)) {
      $form['pubdate_field']['#description'] .= '<br />' . t('<strong>Try adding a date field to your views fields to enable an option here.</strong>');
    }

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

    $form['guid_perma'] = array(
      '#type' => 'checkbox',
      '#default_value' => $this->options['guid_perma'],
      '#title' => t('GUID is a permalink'),
    );
    $weight = 0;
    foreach (element_children($form) as $child) {
      if ($child !== 'guid_perma') {
        $form[$child]['#weight'] = $weight++;
      }
      if ($child == 'guid_field') {
        $form['guid_perma']['#weight'] = $weight++;
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

    $rss_namespaces = array(
      'xmlns:itunes' => 'http://www.itunes.com/dtds/podcast-1.0.dtd',
    );

    $rss_elements = array();


    $this->view->style_plugin->namespaces = array_merge($this->view->style_plugin->namespaces, $rss_namespaces);

    $item = new stdClass;

    // Add fields to the item that are special
    $item->title = $this->render_field('title_field');
    $item->description = '';
    $item->link = '';

    // Add all the other elements, defined in options.
    foreach ($this->option_definition() as $option => $options) {
      if (isset($options['ffpc']['xml_element'])) {
        if ($value = $this->render_field($option)) {
          // Do we need to pop it on the $item, or in the $rss_elements
          if (empty($options['ffpc']['on_object'])) {
            $rss_elements[] = array('key' => $options['ffpc']['xml_element'], 'value' => $value);
          }
          else {
            $item->{$options['ffpc']['xml_element']} = $value;
          }
        }
      }
    }

    // Handle the GUID permalink
    foreach ($rss_elements as $k => $element) {
      if ($element['key'] == 'guid') {
        $rss_elements[$k]['attributes']['isPermaLink'] = empty($this->options['guid_perma']) ? 'false' : 'true';
      }
    }

    $item->elements = $rss_elements;

    if (!empty($this->options['enclosure_field'])) {
      $this->render_handled_field($item, $this->options['enclosure_field'], 'file');
    }
    if (!empty($this->options['pubdate_field'])) {
      $this->render_handled_field($item, $this->options['pubdate_field'], 'date');
    }

    return theme($this->theme_functions(),
      array(
        'view' => $this->view,
        'options' => $this->options,
        'row' => $item
      ));
  }

  protected function render_handled_field($item, $option, $type) {
    list($option_field, $option_handler) = explode(':', $option);
    $field_handler = $this->view->display_handler->get_handler('field', $option_field);
    $ffpc_handlers = ffpc_get_handled_field_types($type);
    $view_values = $this->view->result[$this->view->row_index];
    if (isset($ffpc_handlers[$option_handler])) {
      $ffpc_handler = $ffpc_handlers[$option_handler];
      // Make sure this still a valid field.
      if (isset($ffpc_handler['identification callback']) && function_exists($ffpc_handler['identification callback'])) {
        $function = $ffpc_handler['identification callback'];

        if ($function($field_handler)) {
          // We can now render this field too.
          if (isset($ffpc_handler['render callback']) && function_exists($ffpc_handler['render callback'])) {
            $render_callback = $ffpc_handler['render callback'];
            $render_callback($item, $view_values, $field_handler);
          }
        }
      }
    }

  }
}
