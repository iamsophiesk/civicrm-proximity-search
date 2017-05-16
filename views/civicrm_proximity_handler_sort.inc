<?php

/**
 * Views sorting handler
 */
class civicrm_proximity_handler_sort extends views_handler_sort {

  /**
   *
   */
  public function option_definition() {

    $options = parent::option_definition();

    $options['location_provider'] = array('default' => '');
    return $options;

  }

  /**
   * Basic options for all sort criteria.
   */
  public function options_form(&$form, &$form_state) {

    if ($handlers = $this->view->display_handler->get_handlers('filter')) {
      $options = array();

      foreach ($handlers as $name => $handler) {
        $definition = $handler->definition['handler'];

        if ($definition == 'civicrm_proximity_handler_filter_circle') {
          $group = $handler->definition['group'];
          $title = $handler->definition['title'];
          $admin = $handler->admin_summary();

          $options[$name] = $group . ': ' . $title . ' (' . $admin . ')';
        }
      }

      $form['location_provider'] = array(
        '#title'   => t('Location provider'),
        '#type'    => 'select',
        '#options' => $options,
      );
    }

    parent::options_form($form, $form_state);

  }

  /**
   *
   */
  public function query() {

    $this->ensure_my_table();

    $lp      = $this->options['location_provider'];
    $handler = $this->view->display_handler->get_handler('filter', $lp);
    $table   = $this->query->table_queue[$handler->table_alias()];

    if ($handler && $handler->value['location'] && isset($table)) {
      $alias = $handler->table_alias();
      $field = $this->real_field;
      $order = $this->options['order'];
      $this->query->add_orderby($alias, $field, $order);
    }

  }

}
