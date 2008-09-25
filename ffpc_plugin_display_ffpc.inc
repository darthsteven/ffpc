<?php
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