<?php
/**
 * Default style plugin to render a Podcast
 */
class ffpc_plugin_style_podcast extends views_plugin_style_rss {
  function option_definition() {
    $options = parent::option_definition();

    $options['itunes-owner'] = array('default' => '', 'translatable' => TRUE);
    $options['itunes-subtitle'] = array('default' => '', 'translatable' => TRUE);
    $options['itunes-author'] = array('default' => '', 'translatable' => TRUE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['itunes-owner'] = array(
      '#type' => 'textfield',
      '#title' => t('iTunes Owner'),
      '#default_value' => $this->options['itunes-owner'],
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
  }
  
}
