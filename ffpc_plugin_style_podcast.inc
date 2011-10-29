<?php
/**
 * Default style plugin to render a Podcast
 */
class ffpc_plugin_style_podcast extends views_plugin_style_rss {
  function option_definition() {
    $options = parent::option_definition();

    $options['copyright'] = array('default' => '', 'translatable' => TRUE);
    $options['description'] = array('default' => '', 'translatable' => TRUE);
    $options['category'] = array('default' => '', 'translatable' => TRUE);
    $options['itunes-image'] = array('default' => '');
    $options['itunes-explicit'] = array('default' => '');
    $options['itunes-owner-name'] = array('default' => '', 'translatable' => TRUE);
    $options['itunes-owner-email'] = array('default' => variable_get('site_mail', ini_get('sendmail_from')));
    $options['itunes-subtitle'] = array('default' => '', 'translatable' => TRUE);
    $options['itunes-summary'] = array('default' => '', 'translatable' => TRUE);
    $options['itunes-author'] = array('default' => '', 'translatable' => TRUE);
    $options['itunes-category'] = array('default' => array(), 'translatable' => FALSE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['copyright'] = array(
      '#type' => 'textfield',
      '#title' => t('Copyright'),
      '#default_value' => $this->options['copyright'],
      '#description' => t('This will appear in the RSS feed itself.'),
    );

    $form['description'] = array(
      '#type' => 'textarea',
      '#title' => t('Description'),
      '#default_value' => $this->options['description'],
      '#description' => t('This will appear in the RSS feed itself.'),
    );

    $form['category'] = array(
      '#type' => 'textfield',
      '#title' => t('Category'),
      '#default_value' => $this->options['category'],
      '#description' => t('This will appear in the RSS feed itself.'),
    );

    $form['itunes-image'] = array(
      '#type' => 'textfield',
      '#title' => t('iTunes Image'),
      '#default_value' => $this->options['itunes-image'],
      '#description' => t('This can be a URI or path from the Drupal root directory. This will appear in the RSS feed itself.'),
    );

    $form['itunes-explicit'] = array(
      '#type' => 'select',
      '#title' => t('iTunes Explicit rating'),
      '#default_value' => $this->options['itunes-explicit'],
      '#options' => array(
        '' => t('No (no indicator shown)'),
        'yes' => t('Yes'),
        'clean' => t('Clean'),
      ),
      '#description' => t('This will appear in the RSS feed itself.'),
    );

    $form['itunes-owner-name'] = array(
      '#type' => 'textfield',
      '#title' => t('iTunes Owner - Name'),
      '#default_value' => $this->options['itunes-owner-name'],
      '#description' => t('This will appear in the RSS feed itself.'),
    );

    $form['itunes-owner-email'] = array(
      '#type' => 'textfield',
      '#title' => t('iTunes Owner - Email'),
      '#default_value' => $this->options['itunes-owner-email'],
      '#description' => t('This will appear in the RSS feed itself.'),
    );

    $form['itunes-subtitle'] = array(
      '#type' => 'textfield',
      '#title' => t('iTunes Subtitle'),
      '#default_value' => $this->options['itunes-subtitle'],
      '#description' => t('This will appear in the RSS feed itself.'),
    );

    $form['itunes-author'] = array(
      '#type' => 'textfield',
      '#title' => t('iTunes Author'),
      '#default_value' => $this->options['itunes-author'],
      '#description' => t('This will appear in the RSS feed itself.'),
    );

    $form['itunes-keywords'] = array(
      '#type' => 'textarea',
      '#title' => t('iTunes Keywords'),
      '#default_value' => $this->options['itunes-keywords'],
      '#description' => t('This tag allows users to search on a maximum of 12 text keywords. Use commas to separate keywords.'),
    );

    $form['itunes-summary'] = array(
      '#type' => 'textarea',
      '#title' => t('iTunes Summary'),
      '#default_value' => $this->options['itunes-summary'],
      '#description' => t('This will appear in the RSS feed itself.'),
    );

    $form['itunes-category'] = array(
      '#type' => 'checkboxes',
      '#title' => t('iTunes Categories'),
      '#default_value' => $this->options['itunes-category'],
      '#description' => t('You should select no more than three categories.'),
      '#options' => ffpc_itunes_categories_options(),
    );
  }
}
