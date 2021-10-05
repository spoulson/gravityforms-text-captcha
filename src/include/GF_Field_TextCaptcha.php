<?php
class GF_Field_TextCaptcha extends GF_Field {
  public $type = 'text_captcha';
  public $length = 6;
  public $figlet_args = '-w 1000';
  public $font = 'roman';
  public $allowed_chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';

  function __construct($data = []) {
    parent::__construct($data);

    // Parse system configuration.
    if (isset($data['GF_TEXT_CAPTCHA_LENGTH'])) {
      $cfg_length = intval($data['GF_TEXT_CAPTCHA_LENGTH']);
      if ($cfg_length > 0) {
        $this->length = $cfg_length;
      }
    }

    if (isset($data['GF_TEXT_CAPTCHA_FONT'])) {
      $this->font = $data['GF_TEXT_CAPTCHA_FONT'];
    }

    if (isset($data['GF_TEXT_CAPTCHA_FIGLET_ARGS'])) {
      $this->figlet_args = $data['GF_TEXT_CAPTCHA_FIGLET_ARGS'];
    }

    if (isset($data['GF_TEXT_CAPTCHA_ALLOWED_CHARS'])) {
      $this->allowed_chars = $data['GF_TEXT_CAPTCHA_ALLOWED_CHARS'];
    }

    $this->salt = AUTH_SALT;
    $this->key = AUTH_KEY;
    $this->fonts_path = plugin_dir_path(__FILE__) . 'fonts';
  }

  // Read global configuration.
  // Returns array that can be passed to constructor.
  public static function initialize_config() {
    $args = [];

    if (defined('GF_TEXT_CAPTCHA_LENGTH')) {
      $args['GF_TEXT_CAPTCHA_LENGTH'] = GF_TEXT_CAPTCHA_LENGTH;
    }

    if (defined('GF_TEXT_CAPTCHA_FONT')) {
      $args['GF_TEXT_CAPTCHA_FONT'] = GF_TEXT_CAPTCHA_FONT;
    }

    if (defined('GF_TEXT_CAPTCHA_FIGLET_ARGS')) {
      $args['GF_TEXT_CAPTCHA_FIGLET_ARGS'] = GF_TEXT_CAPTCHA_FIGLET_ARGS;
    }

    if (defined('GF_TEXT_CAPTCHA_ALLOWED_CHARS')) {
      $args['GF_TEXT_CAPTCHA_ALLOWED_CHARS'] = GF_TEXT_CAPTCHA_ALLOWED_CHARS;
    }

    return $args;
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
    $captcha_str = $this->generate_captcha_str();
    $code = $this->generate_captcha_code($captcha_str);

    $captcha_art = htmlentities($this->make_figlet_image($captcha_str));
    $captcha_html = <<<EOF
<div class="gfield_text_captcha_str">
<pre>${captcha_art}</pre>
</div>
EOF;

    $input_attrs = "";
    if ($this->is_form_editor()) {
      $input_attrs = " disabled=\"disabled\"";
    }

    $input_html = <<<EOF
<div class="ginput_container ginput_container_text_captcha"">
<input type="text" id="input_${id}" name="input_${id}[]" tabindex="1"${input_attrs} />
</div>
EOF;

    $code_hex = bin2hex($code);
    $hidden_html = <<<EOF
<input type="hidden" name="input_${id}[]" value="${code_hex}" class="gform_hidden" />
EOF;

    return $captcha_html . $input_html . $hidden_html;
  }

  // Return a newly generates CAPTCHA string.
  private function generate_captcha_str() {
    $captcha_str = '';
    $rand_max = strlen($this->allowed_chars) - 1;

    for ($i = 0; $i < $this->length; $i++) {
      $r = random_int(0, $rand_max);
      $captcha_str .= substr($this->allowed_chars, $r, 1);
    }

    return $captcha_str;
  }

  // Generate encrypted code from CAPTCHA string.
  // Returns an unencoded binary string.
  private function generate_captcha_code($captcha_str) {
    // TODO: Use HMAC instead.
    $secret = $this->key;
    $salt = openssl_random_pseudo_bytes(4);
    $iv = hash_pbkdf2('sha1', $this->salt, 0, 1, 16, true);
    $key = hash_pbkdf2('sha1', $secret, 0, 1, 32, true);
    $plaintext = $salt . $captcha_str;
    return openssl_encrypt($plaintext, 'aes-256-cbc', $key, OPENSSL_RAW_DATA, $iv);
  }

  // Verify CAPTCHA string with code.
  // $code must be an unencoded binary string.
  // Returns true if CAPTCHA matches.
  private function verify_captcha($captcha_str, $code) {
    $secret = $this->key;
    $ciphertext = $code;
    $iv = hash_pbkdf2('sha1', $this->salt, 0, 1, 16, true);
    $key = hash_pbkdf2('sha1', $secret, 0, 1, 32, true);
    $plaintext = openssl_decrypt($ciphertext, 'aes-256-cbc', $key, OPENSSL_RAW_DATA, $iv);
    if ($plaintext === false) {
      error_log('verify_captcha() decrypt failed');
      return false;
    }
    $captcha_verify = substr($plaintext, 4);
    return $captcha_verify === $captcha_str;
  }

  // Validate form input.
  public function validate($value, $form) {
    if (!is_array($value) || count($value) !== 2) {
      error_log('Invalid value submitted to Text CAPTCHA field.');
      return false;
    }

    $captcha_str = $value[0];
    $code = hex2bin($value[1]);

    if (!$this->verify_captcha($captcha_str, $code)) {
      $this->failed_validation = true;
      $this->validation_message = 'CAPTCHA verification failed';
      return;
    }
  }

  // Render figlet text.
  // Convert input string to text-based ASCII art.
  private function make_figlet_image($str) {
    $fonts_path = $this->fonts_path;
    $cmd = "figlet -d ${fonts_path} " . $this->figlet_args . ' -f ' . $this->font . ' ' . escapeshellcmd($str);
    // TODO: Handle error.
    return shell_exec($cmd);
  }
}
