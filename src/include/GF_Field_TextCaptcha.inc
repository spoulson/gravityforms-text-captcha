<?php
$path = plugin_dir_path(__FILE__);
require_once($path . './GF_TextCaptcha_Config.inc');
require_once($path . './GF_TextCaptcha_Generator.inc');

class GF_Field_TextCaptcha extends GF_Field {
  // Gravity Forms field type name.
  public $type = 'text_captcha';

  // GF_TextCaptcha_Generator singleton object.
  public static $generator;

  function __construct($data = []) {
    parent::__construct($data);

    // Ensure generator singleton is created.
    if (empty(GF_Field_TextCaptcha::$generator)) {
      $cfg = new GF_TextCaptcha_Config();
      $cfg->initialize();
      GF_Field_TextCaptcha::$generator = new GF_TextCaptcha_Generator($cfg);
    }
  }

  // Scripts to be included for this field type in the form editor.
  // @return {string} JavaScript to emit.
  public function get_form_editor_inline_script_on_page_render() {
    return <<<EOF
function SetDefaultValues_text_captcha(field) {
  field.label = 'CAPTCHA';
}
EOF;
  }

  // Scripts to be included with the form init scripts ont he front-end.
  // @param {array} $form Form object.
  // @return {string} JavaScript to emit.
  public function get_form_inline_script_on_page_render($form) {
    $form_id = (int) rgar($form, 'id');
    $field_id = $this->id;
    return <<<EOF
window.GF_TextCaptcha = {
  reloadCaptcha: function() {
    // Find field element.
    elemSelector = '#field_${form_id}_${field_id}';
    jQuery(elemSelector).each(function (i, elem) {
      jQuery(elem)
        .addClass('captcha-loading')
        .find('INPUT.try-another')
        .prop('disabled', true);

      // Call API for new CAPTCHA.
      jQuery.ajax({
        url: '/wp-json/gravityforms-text-captcha/v1/form/${form_id}/captcha/${field_id}/',
        type: 'GET',
        dataType: 'json',
        success: function (data) {
          if (data == undefined || data == null) {
            console.error('Error requesting new CAPTCHA: No data returned');
            return;
          }

          html = data.html;
          if (html == undefined || html == null) {
            console.error('Error requesting new CAPTCHA: Invalid data returned');
            return;
          }

          jQuery(elem).find('.gfield_text_captcha_container').replaceWith(html);
        },
        error: function (error) {
          console.error('Error requesting new CAPTCHA: Request error');
        },
        complete: function () {
          jQuery(elem)
            .removeClass('captcha-loading')
            .find('.ginput_container_text_captcha > INPUT, INPUT.try-another')
            .prop('disabled', false);
        }
      });
    });
  }
};

window.GF_TextCaptcha.reloadCaptcha();
EOF;
  }

  // Get field title in form editor.
  // @return {string} Field title name.
  public function get_form_editor_field_title() {
    return esc_attr__('Text CAPTCHA', 'gravityforms');
  }

  // Get form editor new field button settings.
  // @return {array} Field definition for form editor.
  public function get_form_editor_button() {
    return [
      'group' => 'advanced_fields',
      'text' => $this->get_form_editor_field_title()
    ];
  }

  // Get form editor field settings.
  // These are proprietary options supported by Gravity Forms.
  // @return {array} List of field option names.
  public function get_form_editor_field_settings() {
    return [
      'label_setting',
      'description_setting',
      'rules_setting',
      'label_placement_setting',
      'error_message_setting',
      'css_class_setting',
      'admin_label_setting',
      'visibility_setting',
      'conditional_logic_field_setting'
    ];
  }

  // Render field HTML.
  // @param {array} $form Form object.
  // @param {string|array} $value Field value.
  // @param {array|null} $entry Entry object currently being edited.
  // @return {string} HTML output.
  public function get_field_input($form, $value = '', $entry = null) {
    $is_editmode = $this->is_form_editor();
    return GF_Field_TextCaptcha::$generator->generate_html($this->id, $is_editmode, false);
  }

  // Validate form input.
  // @return {bool} True if valid.
  public function validate($value, $form) {
    if (!is_array($value) || count($value) !== 2) {
      error_log('Invalid value submitted to Text CAPTCHA field.');
      return false;
    }

    $captcha_str = $value[0];
    $code = hex2bin($value[1]);

    if (!GF_Field_TextCaptcha::$generator->verify_captcha($captcha_str, $code)) {
      $this->failed_validation = true;
      $this->validation_message = 'CAPTCHA verification failed';
      return;
    }
  }
}
