<?php

/**
 * Return the specially handled field types supported and how to render them.
 *
 * Filefield podcaster needs to know which fields are eligable to be used for
 * the enclosure and date of the podcast feed. This is done by declaring which
 * field types are handled, a identification callback and a render callback for
 * each field type. During the rendering of the podcast feed the render callback
 * will be called with the current row and should add a rendered value to the
 * rss elements.
 *
 * @return
 *   An array keyed by the field type that is being handled, which can be one of
 *   'date' or 'file'. Values of the array should be arrays themselves, keys
 *   are a unique internal name for the handler and the value is a array
 *   containing:
 *   - 'identification callback': A function to determine if a given views
 *     handler can be handled by this handler. Return TRUE if it can be handled,
 *     FALSE otherwise.
 *     This function will be passed the following:
 *     - '$views_handler': The views handler object under consideration. See
 *       ffpc_file_field_identify() for an example.
 *   - 'render callback': The function to call to render the enclosure field.
 *     This function will be passed the following:
 *     - '$item': A reference to the item being rendered, that will be passed to
 *       theme_views_view_row_rss() for rendering.
 *     - '$value': The views result corresponding to this item.
 *     - '$field_handler': The views field handler corresponding to this field.
 *     You should see ffpc_file_field_render() for an example of a
 *     render callback.
 */
function hook_ffpc_handled_field_types() {
  $fields = array();

  $fields['file']['file_field'] = array(
    'identification callback' => 'ffpc_file_field_identify',
    'render callback' => 'ffpc_file_field_render',
  );
  $fields['date']['date_property'] = array(
    'identification callback' => 'ffpc_date_property_identify',
    'render callback' => 'ffpc_date_property_render',
  );

  return $fields;
}

/**
 * Alter the handled field types supported by other modules.
 */
function hook_ffpc_file_field_types_alter(&$types) {

}
