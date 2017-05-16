<?php

/**
 * Distance field handler
 */
class civicrm_proximity_handler_field extends views_handler_field_numeric {

  /**
   * Overwrite views_handler_field_numeric::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();

    $options['location_provider'] = array('default' => '');
    $options['set_precision']     = array('default' => TRUE);
    $options['precision']         = array('default' => 1);
    $options['hide_empty']        = array('default' => TRUE);

    $this->definition['float'] = TRUE;

    return $options;
  }

  /**
   * Basic options for all sort criteria
   * Overwrite views_handler_field_numeric::options_form().
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    if ($handlers = $this->view->display_handler->get_handlers('filter')) {
      $options = array();

      foreach ($handlers as $name => $handler) {
        $handler_def = $handler->definition['handler'];
        if ($handler_def == 'civicrm_proximity_handler_filter_circle') {
          $group = $handler->definition['group'];
          $title = $handler->definition['title'];
          $admin = $handler->admin_summary();

          $options[$name] = $group . ': ' . $title . ' (' . $admin . ')';
        }
      }

      $form['location_provider'] = array(
        '#title'   => t('Search from'),
        '#type'    => 'select',
        '#options' => $options,
      );
    }
  }

  /**
   * Overwrite views_handler_field_numeric::query().
   */
  public function query() {
    $lp      = $this->options['location_provider'];
    $handler = $this->view->display_handler->get_handler('filter', $lp);
    $alias   = $handler->table_alias();

    if ($handler
    && $handler->value['location']
    && isset($this->query->table_queue[$alias])) {
      $table = $this->options['table'];
      $field = $this->view->query->fields['civicrm_address_distance']['field'];

      $this->field_alias = $this->query->add_field($table, $field);
    }
  }

  /**
   * Overwrite views_handler_field_numeric::render().
   */
  public function render($values) {
    $value = $values;

    if (isset($value->distance)) {

      // Get the handler.
      $loc_prov = $this->options['location_provider'];
      $handler = $this->view->display_handler->get_handler('filter', $loc_prov);

      // Get the unit of measurement.
      $exp_unit = $handler->value[$handler->options['expose']['unit']];
      $hid_unit = $handler->value['unit'];
      $unit     = $handler->options['exposed'] ? $exp_unit : $hid_unit;
      $distance = $value->distance;

      // Convert back to kilometers.
      if ($unit !== "km" && $unit !== "kilometers") {
        $distance = civicrm_proximity_measurement_units_convert_back($unit, $distance);
      }

      if (!empty($this->options['set_precision'])) {
        $p = $this->options['precision'];
        $d = $this->options['decimal'];
        $s = $this->options['separator'];

        $value = round($distance, $p);
      }
      else {
        $value = number_format($value, 0, '', $this->options['separator']);
      }

      // Check to see if hiding should happen before adding prefix and suffix.
      $hide_empty = $this->options['hide_empty'];
      $empty_zero = $this->options['empty_zero'];

      if ($hide_empty && empty($value) && ($value !== 0 || $empty_zero)) {
        return '';
      }

      $units = civicrm_proximity_get_available_units();
      $pre   = $this->options['prefix'];
      $short = $units[$unit]['short'];
      $suf   = $this->options['suffix'];

      return $this->sanitize_value($pre . $value . ' ' . $short . ' ' . $suf);
    }
  }

}
