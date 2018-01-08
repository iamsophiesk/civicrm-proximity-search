<?php

/**
 * Circular search filter handler
 */
class civicrm_proximity_handler_filter_circle extends civicrm_proximity_handler_filter {

  /**
   * Process operator.
   */
  public function op_process($op = 'simple', $field) {
    parent::op_process($op, $field);

    if (!is_null($this->value['lat']) && !is_null($this->value['lon'])) {
      $latitude = $this->value['lat'];
      $longitude = $this->value['lon'];
      $distance = $this->value['value'];

      $table   = $this->table_alias();
      $formula = $this->table_formula($latitude, $longitude, $table);

      $this->query->add_field($table, 'distance');
      $this->query->add_field(NULL, $formula, 'distance');

      $group = $this->options['group'];

      if ($op == 'simple') {
        $operator = $this->operator;
        $lat_lon = $this->calculate_coordinates($latitude, $longitude, $distance);

        if ($operator == '<') {
          $this->query->add_where(
            $group,
            $table . '.geo_code_1',
            array($lat_lon['lat1'], $lat_lon['lat2']),
            'BETWEEN'
          );

          $this->query->add_where(
            $group,
            $table . '.geo_code_2',
            array($lat_lon['lon1'], $lat_lon['lon2']),
            'BETWEEN'
          );
        }

        $this->query->add_having(
          $group,
          $table . '.distance',
          $distance,
          $operator
        );

      }
      else {
        if ($this->operator == 'between') {
          $between = array($this->value['min'], $this->value['max']);
          $upper = $this->calculate_coordinates($latitude, $longitude, $this->value['max']);

          $this->query->add_where(
            $group,
            $table . '.geo_code_1',
            array($upper['lat1'], $upper['lat2']),
            'BETWEEN'
          );

          $this->query->add_where(
            $group,
            $table . '.geo_code_2',
            array($upper['lon1'], $upper['lon2']),
            'BETWEEN'
          );

          $this->query->add_where(
            $group,
            $table . '.geo_code_1',
            $upper['lat2'],
            '>'
          );

          $this->query->add_where(
            $group,
            $table . '.geo_code_2',
            $upper['lon2'],
            '>'
          );

          $this->query->add_where(
            $group,
            $table . '.distance',
            $between,
            'BETWEEN'
          );
        }
        else {
          $this->query->add_where(
            $group,
            db_or()
              ->condition($table . '.distance', $this->value['min'], '<')
              ->condition($table . '.distance', $this->value['max'], '>')
          );
        }
      }
    }
  }

  /**
   *
   */
  public function table_formula($lat, $lon, $table) {
    $args = array(
      ':lat1' => $table . '.geo_code_1',
      ':lon1' => $table . '.geo_code_2',
      ':lat2' => $lat,
      ':lon2' => $lon,
    );

    $great_circle = strtr(CIVICRM_PROXIMITY_SQL_GREAT_CIRCLE, $args);

    return "$great_circle";
  }

  /**
   *
   */
  public function table_alias() {
    return $this->table_alias;
  }

  /**
   *
   */
  public function formula_alias() {
    return $this->table_alias . '_filter_' . $this->options['id'];
  }

  /**
   * Function to calculate the needed coordinates.
   *
   * @param float $lat:
   *   The starting point latitude.
   * @param float $lon:
   *   The starting point longitude.
   * @param float $dist:
   *   The radius of the search circle.
   */
  public function calculate_coordinates($lat, $lon, $dist) {
    $vals = array();

    $vals['lon1'] = $lon - $dist / abs(cos(deg2rad($lat)) * 111);
    $vals['lon2'] = $lon + $dist / abs(cos(deg2rad($lat)) * 111);

    $vals['lat1'] = $lat - ($dist / 111);
    $vals['lat2'] = $lat + ($dist / 111);

    return $vals;
  }

}
