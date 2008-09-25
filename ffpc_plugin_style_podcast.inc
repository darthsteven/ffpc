<?php
/**
 * Default style plugin to render a Podcast
 */
class ffpc_plugin_style_podcast extends views_plugin_style {
  function render() {
    global $base_url, $language;
    if (empty($this->row_plugin)) {
      vpr('views_plugin_style_default: Missing row plugin');
      return;
    }
    $rows = '';

    foreach ($this->view->result as $row) {
      $rows .= $this->row_plugin->render($row);
    }

    $channel_defaults = array(
      'version'    => '2.0',
      'title'      => variable_get('site_name', 'Drupal'),
      'link'        => $base_url,
      'description' => variable_get('site_mission', ''),
      'language'    => $language->language
    );
    $channel = array();
    $channel = array_merge($channel_defaults, $channel);

    $args = array(
      'itunes:summary' => $this->view->display_handler->render_header(),
    );
  
    $namespaces = array('xmlns:itunes' => 'http://www.itunes.com/dtds/podcast-1.0.dtd');
    $output = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    $output .= "<rss version=\"". $channel["version"] ."\" ". drupal_attributes($namespaces) .">\n";
    $output .= format_rss_channel(
      $this->view->get_title(),
      $base_url,
      $this->view->display_handler->render_header(),
      $rows,
      $channel['language'],
      $args
    );
    $output .= "</rss>\n";

    drupal_set_header('Content-Type: application/rss+xml; charset=utf-8');
    print $output;
  }
}