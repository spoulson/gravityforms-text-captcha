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

  // Render JavaScript function that sets form editor default settings.
  public function get_form_editor_inline_script_on_page_render() {
    return <<<EOF
function SetDefaultValues_text_captcha(field) {
field.label = 'CAPTCHA';
}
EOF;
  }

  // Get field title in form editor.
  public function get_form_editor_field_title() {
    return esc_attr__('Text CAPTCHA', 'gravityforms');
  }

  // Get form editor new field button settings.
  public function get_form_editor_button() {
    return [
      'group' => 'advanced_fields',
      'text' => $this->get_form_editor_field_title()
    ];
  }

  // Get form editor field settings.
  // These are proprietary options supported by Gravity Forms.
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
  public function get_field_input($form, $value = '', $entry = null) {
    $id = (int) $this->id;
    $is_editmode = $this->is_form_editor();
    return GF_Field_TextCaptcha::$generator->generate_html($id, $is_editmode);
  }

  // Validate form input.
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
