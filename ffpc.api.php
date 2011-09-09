<?php

/**
 * Return the enclosure field types supported and how to render them.
 *
 * Filefield podcaster needs to know which fields are eligable to be used for
 * the enclosure of the podcast feed. This is done by declaring which field
 * types are handled, and a render callback for each field type. During the
 * rendering of the podcast feed the render callback will be called with details
 * and should add details of the enclosure to the rss elements.
 *
 * @return
 *   An array keyed by the field type that is being handled, values of the array
 *   should be arrays themselves consisting of:
 *   - 'render callback': The function to call to render the enclosure field.
 *     This function will be passed the following:
 *     - '&$rss_elements': A reference to the array of RSS elements being built
 *        up for the current item.
 *     - '$value': The views result corresponding to this item.
 *     - '$field_handler': The views field handler corresponding to this field.
 *     You should see ffpc_file_field_enclosure_render() for an example of a
 *     render callback.
 */
function hook_ffpc_file_field_types() {
  $fields = array();

  $fields['file'] = array(
    'render callback' => 'ffpc_file_field_enclosure_render',
  );

  return $fields;
}

/**
 * Alter the enclosure field types supported by other modules.
 */
function hook_ffpc_file_field_types_alter(&$types) {
  
}
