<?php
/*
Plugin Name: Gravity Forms Text CAPTCHA
Plugin URI:
Description: Text-based CAPTCHA field for Gravity Forms.
Version:     0.1
Author:      Shawn Poulson
Author URI:  https://explodingcoder.com
License:     MIT License
License URI: https://choosealicense.com/licenses/mit/

MIT License

Copyright (c) 2021 Shawn Poulson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

if (class_exists('GF_Field')) {
  class GF_Field_TextCaptcha extends GF_Field {
    public $type = 'text_captcha';

    // TODO: Make length configurable.
    public $length = 6;

    // TODO: Make font configurable.
    public $figlet_font = 'roman';

    public function get_form_editor_inline_script_on_page_render() {
      return <<<EOF
function SetDefaultValues_text_captcha(field) {
  field.label = 'CAPTCHA';
}
EOF;
    }

    public function get_form_editor_field_title() {
      return esc_attr__('Text CAPTCHA', 'gravityforms');
    }

    public function get_form_editor_button() {
      return [
        'group' => 'advanced_fields',
        'text' => $this->get_form_editor_field_title()
      ];
    }

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

    public function get_field_input($form, $value = '', $entry = null) {
      $id = (int) $this->id;
      $captcha_str = $this->generate_captcha_str();
      $code = $this->get_captcha_code($captcha_str);

      $captcha_art = htmlentities($this->make_figlet_image($captcha_str));
      $captcha_html = <<<EOF
<div class="CAPTCHA"><pre>${captcha_art}</pre></div>
EOF;

      $input_attrs = "";
      if ($this->is_form_editor()) {
        $input_attrs = " disabled=\"disabled\"";
      }

      $input_html = <<<EOF
<div class="CAPTCHA_input"><input type="text" name="input_${id}[]" tabindex="1"${input_attrs} /></div>
EOF;

      $code_hex = bin2hex($code);
      $hidden_html = <<<EOF
<input type="hidden" name="input_${id}[]" value="${code_hex}" class="gform_hidden" />
EOF;

      return $captcha_html . $input_html . $hidden_html;
    }

    // Return string of allowed CAPTCHA characters.
    private function get_allowed_chars() {
      return 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    }

    // Return a newly generates CAPTCHA string.
    private function generate_captcha_str() {
      $allowed_chars = $this->get_allowed_chars();
      $captcha_str = '';
      $rand_max = strlen($allowed_chars) - 1;

      for ($i = 0; $i < $this->length; $i++) {
        $r = random_int(0, $rand_max);
        $captcha_str .= substr($allowed_chars, $r, 1);
      }

      return $captcha_str;
    }

    // Get CAPTCHA secret from configuration.
    private function get_captcha_secret() {
      return AUTH_KEY;
    }

    // Generate encrypted code from CAPTCHA string.
    // Returns an unencoded binary string.
    private function get_captcha_code($captcha_str) {
      // TODO: Use HMAC instead.
      $secret = $this->get_captcha_secret();
      $salt = openssl_random_pseudo_bytes(4);
      $iv = hash_pbkdf2('sha1', AUTH_SALT, 0, 1, 16, true);
      $key = hash_pbkdf2('sha1', $secret, 0, 1, 32, true);
      $plaintext = $salt . $captcha_str;
      return openssl_encrypt($plaintext, 'aes-256-cbc', $key, OPENSSL_RAW_DATA, $iv);
    }

    // Verify CAPTCHA string with code.
    // $code must be an unencoded binary string.
    // Returns true if CAPTCHA matches.
    private function verify_captcha($captcha_str, $code) {
      $secret = $this->get_captcha_secret();
      $ciphertext = $code;
      $iv = hash_pbkdf2('sha1', AUTH_SALT, 0, 1, 16, true);
      $key = hash_pbkdf2('sha1', $secret, 0, 1, 32, true);
      $plaintext = openssl_decrypt($ciphertext, 'aes-256-cbc', $key, OPENSSL_RAW_DATA, $iv);
      if ($plaintext === false) {
        error_log('verify_captcha() decrypt failed');
        return false;
      }
      $captcha_verify = substr($plaintext, 4);
      return $captcha_verify === $captcha_str;
    }

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

    private function get_fonts_path() {
      return plugin_dir_path(__FILE__) . DIRECTORY_SEPARATOR . 'fonts';
    }

    // Convert input string to text-based ASCII art.
    private function make_figlet_image($str) {
      $font_path = $this->get_fonts_path();
      $cmd = 'figlet -f ' . $this->figlet_font . " -d ${font_path} -w 1000 " . escapeshellcmd($str);
      // TODO: Handle error.
      return shell_exec($cmd);
    }
  }

  GF_Fields::register(new GF_Field_TextCaptcha());
}
