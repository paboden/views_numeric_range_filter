<?php

/**
 * @file
 * Definition of views_handler_filter_numeric_range.
 */

/**
 * Simple filter to handle numeric range filters
 *
 * @ingroup views_field_handlers
 */
class views_handler_filter_numeric_range extends views_handler_filter_numeric {

  function operators() {
    $operators = parent::operators();

    $operators['range'] = array(
      'title' => t('Numeric range'),
      'method' => 'op_range',
      'short' => t('Range'),
      'values' => 1,
    );

    return $operators;
  }

  /**
   * Provide a list of all the numeric operators
   */
  function operator_options($which = 'title') {
    $options = parent::operator_options();

    foreach ($this->operators() as $id => $info) {
      $options[$id] = $info[$which];
    }

    return $options;
  }

  function operator_values($values = 1) {
    $options = parent::operator_values();

    foreach ($this->operators() as $id => $info) {
      if ($info['values'] == $values) {
        $options[] = $id;
      }
    }

    return $options;
  }

  function has_extra_options() { return TRUE; }

  function option_definition() {
    $options = parent::option_definition();

    $options['range_min'] = array('default' => '');
    $options['range_max'] = array('default' => '');
    $options['range_operator'] = array('default' => 'integer');

    return $options;
  }

  function extra_options_form(&$form, &$form_state) {
    parent::extra_options_form($form, $form_state);

    $form['range_min'] = array(
      '#type' => 'textfield',
      '#title' => t('- Minus'),
      '#default_value' => $this->options['min'],
      '#description' => t('Value minus this amount starts the search'),
    );

    $form['range_max'] = array(
      '#type' => 'textfield',
      '#title' => t('+ Plus'),
      '#default_value' => $this->options['max'],
      '#description' => t('Value plus this amount ends the search'),
    );

    $form['range_operator'] = array(
      '#type' => 'select',
      '#title' => t('Operator'),
      '#options' => array(
        'integer' => t('Integer'),
        'percent' => t('Percent'),
        'multiple' => t('Multiple'),
      ),
      '#default_value' => $this->options['operator'],
      '#description' => t('The operator to use'),
    );
  }

  function op_range($field) {
    $operator = $this->options['range_operator'];
    $minus = $this->options['range_min'];
    $plus = $this->options['range_max'];
    $value = $this->value['value'];

    switch($operator) {
      case 'integer' :
        $min = $value - $minus;
        $max = $value + $plus;
        break;
      case 'percent' :
        $min = $value - (($minus * 0.01) * $value);
        $max = (($plus * 0.01) * $value) + $value;
        break;
      case 'multiple' :
        $min = ($value * $minus) - $value;
        $max = ($value * $plus) + $value;
        break;
    }

    if ($this->operator == 'range') {
      $this->query->add_where($this->options['group'], $field, array($min, $max), 'BETWEEN');
    }
    else {
      $this->query->add_where($this->options['group'], db_or()->condition($field, $min, '<=')->condition($field, $max, '>='));
    }
  }
}
