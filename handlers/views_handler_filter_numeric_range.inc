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

  function option_definition() {
    $options = parent::option_definition();
    $options['value'] = array(
      'contains' => array(
        'range_values' => array('default' => ''),
        'range_operator' => array('default' => ''),
        'range_low' => array('default' => ''),
        'range_high' => array('default' => ''),
        'range' => array('default' => ''),
      ),
    );
    return $options;
  }

  function operators() {
    $operators = parent::operators();
    $operators += array(
      'range' => array(
        'title' => t('Numeric range'),
        'method' => 'op_range',
        'short' => t('Range'),
        'values' => 2,
      ),
    );
    return $operators;
  }

  /**
   * Provide a list of all the numeric operators
   */
  function operator_options($which = 'title') {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      $options[$id] = $info[$which];
    }

    return $options;
  }

  function operator_values($values = 1) {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      if ($info['values'] == $values) {
        $options[] = $id;
      }
    }

    return $options;
  }

  function op_range($field) {

    $value = $this->value['value'];
    //$range = $this->value['range'];
    //$operator = $this->value['operator'];

    switch ($operator) {
      // If default number, treat as just plus/minus a single value
      case 'default' :
        $range = $range;
        break;
      // If percent, range becomes a percent value of the original value
      case 'percent' :
        $range = ($range * 0.01) / $value;
        break;
      // If percent, range becomes a percent value of the original value
      case 'multiply' :
        $range = $range * $value;
        break;
    }

    $temp_ramge = $this->value['value'];
    $range = ($temp_ramge * 0.01) / $value;
    $min = $value - $range;
    $max = $value + $range;

    if ($this->operator == 'range') {
      $this->query->add_where($this->options['group'], $field, array($min, $max), 'BETWEEN');
    }
    else {
      $this->query->add_where($this->options['group'], db_or()->condition($field, $min, '<=')->condition($field, $max, '>='));
    }
  }

  /**
   * Provide a simple textfield for equality
   */
  function value_form(&$form, &$form_state) {

    // We have to make some choices when creating this as an exposed
    // filter form. For example, if the operator is locked and thus
    // not rendered, we can't render dependencies; instead we only
    // render the form items we need.
    $which = 'all';
    if (!empty($form['operator'])) {
      $source = ($form['operator']['#type'] == 'radios') ? 'radio:options[operator]' : 'edit-options-operator';
    }

    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];

      if (empty($this->options['expose']['use_operator']) || empty($this->options['expose']['operator_id'])) {
        // exposed and locked.
        $which = in_array($this->operator, $this->operator_values(2)) ? 'minmax' : 'value';
      }
      else {
        $source = 'edit-' . drupal_html_id($this->options['expose']['operator_id']);
      }
    }

    $form['value']['range_values'] = array(
      '#type' => 'checkbox',
      '#title' => 'Set range values',
      '#default_value' => $this->value['range_values'],
      '#states' => array(
        'visible' => array(
          '.form-item-options-operator select' => array('value' => 'range')
        ),
      ),
    );

    $form['value']['range'] = array(
      '#type' => 'textfield',
      '#title' => 'Value',
      '#default_value' => $this->value['range_low'],
      '#states' => array(
        'visible' => array(
          '.form-item-options-value-range-values input' => array('checked' => FALSE)
        ),
      ),
    );

    $form['value']['range_low'] = array(
      '#type' => 'textfield',
      '#title' => '- Value',
      '#default_value' => $this->value['range_low'],
      '#states' => array(
        'visible' => array(
          '.form-item-options-value-range-values input' => array('checked' => TRUE)
        ),
      ),
    );

    $form['value']['range_high'] = array(
      '#type' => 'textfield',
      '#title' => '+ Value',
      '#default_value' => $this->value['range_high'],
      '#states' => array(
        'visible' => array(
          '.form-item-options-value-range-values input' => array('checked' => TRUE)
        ),
      ),
    );
    $form['value']['range_operator'] = array(
      '#type' => 'select',
      '#title' => 'Operator',
      '#options' => array(
        'value' => t('Value'),
        'percentage' => t('Percentage'),
        'multiplication' => t('Multiplication'),
      ),
      '#default_value' => $this->value['range_operator'],
      '#states' => array(
        'visible' => array(
          '.form-item-options-operator select' => array('value' => 'range')
        ),
      ),
    );

    parent::value_form($form, $form_state);
    
  }

  function op_between($field) {
    if ($this->operator == 'between') {
      $this->query->add_where($this->options['group'], $field, array($this->value['min'], $this->value['max']), 'BETWEEN');
    }
    else {
      $this->query->add_where($this->options['group'], db_or()->condition($field, $this->value['min'], '<=')->condition($field, $this->value['max'], '>='));
    }
  }

}
