<?php

/**
 * @file
 *  Menu callbacks for the administration part.
 */

function paver_admin_groups() {
  $groups = paver_settings_group_load_all();
  $header = array(
    t('Title'),
    t('Actions'),
  );
  $rows = array();
  foreach ($groups as $group) {
    $destination = drupal_get_destination();
    $edit_link = l(t('Edit'), 'admin/config/media/paver/' . $group->pid . '/edit/', array('query' => $destination));
    $delete_link = l(t('Delete'), 'admin/config/media/paver/' . $group->pid . '/delete', array('query' => $destination));
    $row = array(
      'title' => $group->title,
      'actions' => array(
        'data' => implode(' | ', array($edit_link, $delete_link))
      ),
    );
    $rows[] = $row;
  }
  return array(
    'list' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#caption' => t('Paver setting groups'),
    ),
    'add_form' => drupal_get_form('paver_group_form'),
  );
}

/**
 * Form constructor for the paver settings group edit / add feature.
 */
function paver_group_form($form, &$form_state, $paver_group = NULL) {
  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#description' => t('Please input a title used in the administrative area.'),
    '#default_value' => isset($paver_group->title) ? $paver_group->title : '',
  );
  $form['cursorThrottle'] = array(
    '#title' => t('Cursor throttle'),
    '#description' => t('This throttles to the listening of the <em>mousemove</em> events, to prevent jittery panning actions of Paver-enabled panoramas. Defaults to 60fps.'),
    '#default_value' => isset($paver_group->data['cursorThrottle']) ? $paver_group->data['cursorThrottle'] : 60,
    '#type' => 'textfield',
    '#size' => 5,
    '#field_suffix' => t('fps'),
  );
  $form['gracefulFailure'] = array(
    '#title' => t('Graceful failure'),
    '#description' => t('Allows the display of failure message at the desired DOM insertion location.'),
    '#default_value' => isset($paver_group->data['gracefulFailure']) ? $paver_group->data['gracefulFailure'] : TRUE,
    '#type' => 'checkbox',
  );
  $form['gracefulFailureSettings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Graceful failure settings'),
    '#states' => array(
      'visible' => array(
        ':input[name="gracefulFailure"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['gracefulFailureSettings']['failureMessage'] = array(
    '#title' => t('Failure message'),
    '#description' => t('The failure message to display when Paver is not initialized, such that users are not left in the dark in how to navigate/pan through the panorama. This message will appear in mobile devices with no gyroscopic data or no <em>DeviceOrientationEvent</em> support.'),
    '#default_value' => isset($paver_group->data['failureMessage']) ? $paver_group->data['failureMessage'] : '',
    '#type' => 'textfield',
  );
  $form['gracefulFailureSettings']['failureMessageInsert'] = array(
    '#title' => 'Failure message position',
    '#description' => t('The DOM location where the failure message will be inserted. This follows the jQuery convention of inserting new DOM nodes.'),
    '#type' => 'select',
    '#options' => array('before' => t('Before'), 'after' => t('After'), 'append' => t('Append'), 'prepend' => t('Prepend')),
    '#default_value' => isset($paver_group->data['failureMessageInsert']) ? $paver_group->data['failureMessageInsert'] : 'after',
  );
  $form['meta'] = array(
    '#title' => t('Add meta information'),
    '#description' => t('Determines whether a metadata overlay should be displayed. When enabled, the plugin will retrieve the value(s) of the <em>title</em> and/or <em>alt</em>, if declared.'),
    '#default_value' => isset($paver_group->data['meta']) ? $paver_group->data['meta'] : FALSE,
    '#type' => 'checkbox',
  );
  $form['minimumOverflow'] = array(
    '#title' => t('Minimum overflow'),
    '#description' => t('The excess width the panorama must have, in pixels, before Paver kicks in. In other words, this option allows the panorama\'s computed width to exceed that of its parent container by <em>n</em> amount of pixels before we allow for panning.'),
    '#default_value' => isset($paver_group->data['minimumOverflow']) ? $paver_group->data['minimumOverflow'] : 200,
    '#type' => 'textfield',
    '#size' => 5,
    '#field_suffix' => 'pixels',
  );
  $form['mouseSmoothingFunction'] = array(
    '#title' => t('Mouse smoothing function'),
    '#description' => t('Determines how mouse movement can be translated and smoothed into a panning action.'),
    '#type' => 'select',
    '#options' => array('linear' => t('Linear'), 'gaussian' => t('Gaussian'), 'cosine' => t('Cosine'), 'tangent' => t('Tangent')),
    '#default_value' => isset($paver_group->data['mouseSmoothingFunction']) ? $paver_group->data['mouseSmoothingFunction'] : 'linear',
  );
  $form['resizeThrottle'] = array(
    '#title' => t('Resize throttle'),
    '#description' => t('Governs how often the window <em>resize</em> event should be fired. Due to intensive computation requirements of each Paver instance, the window resize event has been highly throttled. Keep this number rather high for performance reasons.'),
    '#default_value' => isset($paver_group->data['resizeThrottle']) ? $paver_group->data['resizeThrottle'] : 500,
    '#type' => 'textfield',
    '#size' => 5,
    '#field_suffix' => 'ms',
  );
  $form['responsive'] = array(
    '#title' => t('Responsive'),
    '#description' => t('Toggles the responsiveness of the panorama image, in the event that the viewport is too wide such that panning is not required. If enabled, Paver will attempt to resize the image to fill the dimensions of the image\'s parent.'),
    '#default_value' => isset($paver_group->data['responsive']) ? $paver_group->data['responsive'] : TRUE,
    '#type' => 'checkbox',
  );
  $form['startPosition'] = array(
    '#title' => t('Start position'),
    '#description' => t('Determines the start position of the panorama when Paver is enabled. By default, this will horizontally center the panorama. The ratio extends from 0 through 1, from the left to the right of the panorama. For example, if you want the panorama to be at the leftmost position upon initialization, use 0. <em>This option will be nullified when gyroscopic data is in use</em>.'),
    '#default_value' => isset($paver_group->data['startPosition']) ? $paver_group->data['startPosition'] : 0.5,
    '#type' => 'textfield',
    '#size' => 5,
  );
  $form['tilt'] = array(
    '#title' => t('Tilt'),
    '#description' => t('When enabled, this option allows Paver to tap to the gyroscopic data available from the device. It translates tilting (front-back and left-right) into panning actions.'),
    '#default_value' => isset($paver_group->data['tilt']) ? $paver_group->data['tilt'] : TRUE,
    '#type' => 'checkbox',
  );
  $form['tilt_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tilt settings'),
    '#states' => array(
      'visible' => array(
        ':input[name="tilt"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['tilt_settings']['gyroscopeThrottle'] = array(
    '#title' => t('Gyroscope throttle'),
    '#description' => t('This throttles to the listening of the <em>deviceorientation</em> events, to prevert jittery panning actions of Paver-enabled panoramas. Defaults to 60fps.'),
    '#default_value' => isset($paver_group->data['gyroscopeThrottle']) ? $paver_group->data['gyroscopeThrottle'] : 60,
    '#type' => 'textfield',
    '#size' => 5,
    '#field_suffix' => t('fps'),
  );
  $form['tilt_settings']['tiltSensitivity'] = array(
    '#title' => t('Tilt Sensitivity'),
    '#description' => t('The number of degrees that has to be tilted in between measurements for the scroller to be displayed on a gyroscope-equipped device. The larger the value, the more rapid tilting action has to be initiated before the scoller appears on a gyroscope-equipped device.'),
    '#default_value' => isset($paver_group->data['tiltSensitivity']) ? $paver_group->data['tiltSensitivity'] : 0.2,
    '#type' => 'textfield',
    '#size' => 5,
  );
  $form['tilt_settings']['tiltSmoothingFunction'] = array(
    '#title' => t('Tilt smoothing function'),
    '#description' => t('Determines how tilting movement can be translated and smoothed into a panning action.'),
    '#type' => 'select',
    '#options' => array('linear' => t('Linear'), 'gaussian' => t('Gaussian'), 'cosine' => t('Cosine'), 'tangent' => t('Tangent')),
    '#default_value' => isset($paver_group->data['tiltSmoothingFunction']) ? $paver_group->data['tiltSmoothingFunction'] : 'linear',
  );
  $form['tilt_settings']['tiltThresholdLandscape'] = array(
    '#title' => t('Tilt threshold for landscape'),
    '#description' => t('The number of degrees from the resting start position where the panning will end, when the device orientation is in landscape mode. For example, with the default value of 24, the user simply has to tilt left/right by 24 degrees to reach the left/rightmost edges of the panorama in landscape mode. This value will be coerced within the range of 0 to 180 degrees.'),
    '#default_value' => isset($paver_group->data['tiltThresholdLandscape']) ? $paver_group->data['tiltThresholdLandscape'] : 24,
    '#type' => 'textfield',
    '#size' => 5,
    '#field_suffix' => 'degrees',
  );
  $form['tilt_settings']['tiltThresholdPortrait'] = array(
    '#title' => t('Tilt threshold for portrait'),
    '#description' => t('The number of degrees from the resting start position where the panning will end, when the device orientation is in portrait mode. For example, with the default value of 12, the user simply has to tilt left/right by 12 degrees to reach the left/rightmost edges of the panorama in portrait mode. This value will be coerced within the range of 0 to 180 degrees.'),
    '#default_value' => isset($paver_group->data['tiltThresholdPortrait']) ? $paver_group->data['tiltThresholdPortrait'] : 12,
    '#type' => 'textfield',
    '#size' => 5,
    '#field_suffix' => 'degrees',
  );
  $form['tilt_settings']['tiltScrollerPersistence'] = array(
    '#title' => t('Tilt scroller position'),
    '#description' => t('Determines the duration, in milliseconds, of which the scroller overlay will appear when the device is being tilted beyond the tiltThreshold (which is device orientation-sensitive).'),
    '#default_value' => isset($paver_group->data['tiltThresholdPortrait']) ? $paver_group->data['tiltThresholdPortrait'] : 500,
    '#type' => 'textfield',
    '#size' => 5,
    '#field_suffix' => 'ms',
  );
  if (!empty($paver_group->pid)) {
    $form['pid'] = array(
      '#type' => 'value',
      '#value' => $paver_group->pid,
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  if (empty($paver_group->pid)) {
    $form_wrapper['add_group'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add settings group'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    ) + $form;
  }
  else {
    $form_wrapper = &$form;
  }
  return $form_wrapper;
}

/**
 * Submit handler for the settings group.
 */
function paver_group_form_submit(&$form, &$form_state) {
  if (!empty($form_state['values']['pid'])) {
    $paver_group = paver_settings_group_load($form_state['values']['pid']);
  }
  else {
    $paver_group = new stdClass();
  }
  $paver_group->title = $form_state['values']['title'];
  foreach (paver_available_data_fields() as $field_name => $field_meta) {
    $paver_group->data[$field_name] = $form_state['values'][$field_name];
  }

  $result = paver_settings_group_save($paver_group);
  if ($result) {
    drupal_set_message(t('The settings group has been saved.'));
  }
  else {
    drupal_set_message(t('There was an error while saving the settings group. Please try again or check the logs for more details.'), 'error');
  }
}

/**
 * Confirm form for removing a paver settings group.
 */
function paver_remove_group_confirm($form, &$form_state, $paver_group) {
  $form['pid'] = array(
    '#type' =>'value',
    '#value' => $paver_group->pid,
  );
  return confirm_form($form, t('Are you sure you want to remove the settings group?'),
    'admin/config/media/paver',
    t('Are you sure you want to remove the paver settings group %group? The action cannot be undone.', array('%group' => $paver_group->title)));
}

/**
 * Submit handler for the group remove confirm form.
 */
function paver_remove_group_confirm_submit($form, &$form_state) {
  paver_settings_groups_delete($form_state['values']['pid']);
  drupal_set_message(t('The settings group has been removed'));
  $form_state['redirect'] = 'admin/config/media/paver';
}
