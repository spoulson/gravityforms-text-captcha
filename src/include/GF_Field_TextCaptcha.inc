<?php
class GF_Field_TextCaptcha extends GF_Field {
  public $type = 'text_captcha';
  public static $length = 6;
  public static $figlet_args = '-w 1000';
  public static $font = 'roman';
  public static $allowed_chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  public static $noise_count = 5;
  public static $noise_colors = ['#0A67A5', '#E98F01', '#C31D25', '#E4E6EC', '#E1C591'];
  public static $noise_opacity_range = [30, 80];
  public static $noise_width_range = [20, 200];
  public static $noise_height_range = [20, 200];
  public static $noise_x_range = [-10, 70];
  public static $noise_y_range = [-10, 50];
  public static $salt;
  public static $key;
  public static $fonts_path;

  function __construct($data = []) {
    parent::__construct($data);
  }

  // Initialize global configuration.
  // Store config in GF_Field_TextCaptcha static members.
  public static function initialize() {
    if (defined('GF_TEXT_CAPTCHA_LENGTH')) {
      $length = intval(GF_TEXT_CAPTCHA_LENGTH);
      if ($length > 0) {
        GF_Field_TextCaptcha::$length = $length;
      }
    }

    if (defined('GF_TEXT_CAPTCHA_FONT')) {
      GF_Field_TextCaptcha::$font = GF_TEXT_CAPTCHA_FONT;
    }

    if (defined('GF_TEXT_CAPTCHA_FIGLET_ARGS')) {
      GF_Field_TextCaptcha::$figlet_args = GF_TEXT_CAPTCHA_FIGLET_ARGS;
    }

    if (defined('GF_TEXT_CAPTCHA_ALLOWED_CHARS')) {
      GF_Field_TextCaptcha::$allowed_chars = GF_TEXT_CAPTCHA_ALLOWED_CHARS;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_COUNT')) {
      GF_Field_TextCaptcha::$noise_count = intval(GF_TEXT_CAPTCHA_NOISE_COUNT);
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_COLORS')) {
      GF_Field_TextCaptcha::$noise_colors = explode(',', GF_TEXT_CAPTCHA_NOISE_COLORS);
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_OPACITY_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_OPACITY_RANGE), 0, 2);
      $range = array_filter($range, intval);
      GF_Field_TextCaptcha::$noise_opacity_range = $range;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_WIDTH_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_WIDTH_RANGE), 0, 2);
      $range = array_map('intval', $range);
      GF_Field_TextCaptcha::$noise_width_range = $range;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_HEIGHT_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_HEIGHT_RANGE), 0, 2);
      $range = array_map('intval', $range);
      GF_Field_TextCaptcha::$noise_height_range = $range;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_X_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_X_RANGE), 0, 2);
      $range = array_map('intval', $range);
      GF_Field_TextCaptcha::$noise_x_range = $range;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_Y_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_Y_RANGE), 0, 2);
      $range = array_map('intval', $range);
      GF_Field_TextCaptcha::$noise_y_range = $range;
    }

    GF_Field_TextCaptcha::$salt = AUTH_SALT;
    GF_Field_TextCaptcha::$key = AUTH_KEY;
    GF_Field_TextCaptcha::$fonts_path = dirname(__FILE__) . '/../fonts';
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

    $noise_html = $this->html_noise();

    $captcha_text = htmlentities($this->make_figlet_image($captcha_str));
    $captcha_html = $this->html_format_captcha_text($captcha_text);

    $input_attrs = "";
    if ($this->is_form_editor()) {
      $input_attrs = " disabled=\"disabled\"";
    }

    $input_html = <<<EOF
<div class="ginput_container ginput_container_text_captcha"">
<input type="text" id="input_${id}" name="input_${id}[]" class="large"${input_attrs} />
</div>
EOF;

    $code_hex = bin2hex($code);
    $hidden_html = <<<EOF
<input type="hidden" name="input_${id}[]" value="${code_hex}" class="gform_hidden" />
EOF;

    return <<<EOF
<div class="gfield_text_captcha_wrapper">
  ${noise_html}${captcha_html}
</div>
${input_html}${hidden_html}
EOF;
  }

  // Format raw output from figlet to HTML.
  // Scramble lines using flex-box ordering.
  private function html_format_captcha_text($input) {
    $lines = explode("\n", rtrim($input));
    $indeces = range(0, count($lines) - 1);
    $unshuffled_indeces = $indeces;

    // Attempt to shuffle $indeces.
    // If PHP is really lucky, it's possible shuffle can make no effective
    // change.  So check and try again if necessary.
    for ($i = 0; $i < 10; $i++) {
      shuffle($indeces);

      // Ensure indeces are shuffled.
      if ($indeces != $unshuffled_indeces) {
        break;
      }
    }

    $out_lines = ['<ul class="gfield_text_captcha_str">'];

    for ($i = 0; $i < count($lines); $i++) {
      $rindex = $indeces[$i];
      $rline = $lines[$rindex];
      $out_lines[] = <<<EOF
<li class="gfield_text_captcha_line" style="order: ${rindex}">${rline}</li>
EOF;
    }

    $out_lines[] = "</ul>";
    return implode("\n", $out_lines);
  }

  // Generate "noise" to appear over/under the CAPTCHA.
  private function html_noise() {
    $colors = GF_Field_TextCaptcha::$noise_colors;
    $output = [];

    for ($i = 0; $i < GF_Field_TextCaptcha::$noise_count; $i++) {
      $opacity = rand(...GF_Field_TextCaptcha::$noise_opacity_range) / 100;
      $width = rand(...GF_Field_TextCaptcha::$noise_width_range); // px
      $height = rand(...GF_Field_TextCaptcha::$noise_height_range); // px
      $left = rand(...GF_Field_TextCaptcha::$noise_x_range); // %
      $top = rand(...GF_Field_TextCaptcha::$noise_y_range); // %
      $z_index = rand(0, 1); // 0 = behind text, 1 = in front of text
      $color = $colors[array_rand($colors)];
      $rotation = rand(0, 359);
      $shape = rand(0, 3);

      switch ($shape) {
      case 0:
        // Circle.
        $output[] = <<<EOF
<div style="position:absolute; width:${width}px; height:${height}px; background:${color}; border-radius:50%; opacity:${opacity}; left:${left}%; top:${top}%; z-index:${z_index}; transform:rotate(${rotation}deg); transform-origin: 50% 50%"></div>
EOF;
        break;
      case 1:
        // Rectangle.
        $output[] = <<<EOF
<div style="position:absolute; width:${width}px; height:${height}px; opacity:${opacity}; left:${left}%; top:${top}%; background:${color}; z-index:${z_index}; transform:rotate(${rotation}deg); transform-origin: 50% 50%"></div>
EOF;
        break;
      case 2:
        // Isosceles triangle.
        $half_width = $width / 2;
        $output[] = <<<EOF
<div style="position:absolute; width:0; height:0; border-left:${half_width}px solid transparent; border-right:${half_width}px solid transparent; border-bottom:${height}px solid ${color}; opacity:${opacity}; z-index:${z_index}; left:${left}%; top:${top}%; transform:rotate(${rotation}deg); transform-origin: 50% 50%"></div>
EOF;
        break;
      case 3:
        // Right triangle.
        $output[] = <<<EOF
<div style="position:absolute; width:0; height:0; border-bottom: ${height}px solid ${color}; border-right: ${width}px solid transparent; opacity:${opacity}; z-index:${z_index}; left:${left}%; top:${top}%; transform:rotate(${rotation}deg); transform-origin: 50% 50%"></div>
EOF;
      }
    }

    return implode("\n", $output);
  }

  // Return a newly generates CAPTCHA string.
  private function generate_captcha_str() {
    $captcha_str = '';
    $rand_max = strlen(GF_Field_TextCaptcha::$allowed_chars) - 1;

    for ($i = 0; $i < GF_Field_TextCaptcha::$length; $i++) {
      $r = random_int(0, $rand_max);
      $captcha_str .= substr(GF_Field_TextCaptcha::$allowed_chars, $r, 1);
    }

    return $captcha_str;
  }

  // Generate encrypted code from CAPTCHA string.
  // Returns an unencoded binary string.
  private function generate_captcha_code($captcha_str) {
    // TODO: Use HMAC instead.
    $secret = GF_Field_TextCaptcha::$key;
    $salt = openssl_random_pseudo_bytes(4);
    $iv = hash_pbkdf2('sha1', GF_Field_TextCaptcha::$salt, 0, 1, 16, true);
    $key = hash_pbkdf2('sha1', $secret, 0, 1, 32, true);
    $plaintext = $salt . $captcha_str;
    return openssl_encrypt($plaintext, 'aes-256-cbc', $key, OPENSSL_RAW_DATA, $iv);
  }

  // Verify CAPTCHA string with code.
  // $code must be an unencoded binary string.
  // Returns true if CAPTCHA matches.
  private function verify_captcha($captcha_str, $code) {
    $secret = GF_Field_TextCaptcha::$key;
    $ciphertext = $code;
    $iv = hash_pbkdf2('sha1', GF_Field_TextCaptcha::$salt, 0, 1, 16, true);
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
    $fonts_path = GF_Field_TextCaptcha::$fonts_path;
    $cmd = "figlet -d ${fonts_path} " . GF_Field_TextCaptcha::$figlet_args .
      ' -f ' . GF_Field_TextCaptcha::$font . ' ' . escapeshellcmd($str);
    // TODO: Handle error.
    return shell_exec($cmd);
  }
}
