<?php
// Text CAPTCHA field configuration class.
class GF_TextCaptcha_Config {
  public $length = 6;
  public $figlet_args = '-w 1000';
  public $font = 'roman';
  public $allowed_chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  public $noise_count = 5;
  public $noise_colors = ['#0A67A5', '#E98F01', '#C31D25', '#E4E6EC', '#E1C591'];
  public $noise_opacity_range = [30, 80];
  public $noise_width_range = [20, 200];
  public $noise_height_range = [20, 200];
  public $noise_x_range = [-10, 70];
  public $noise_y_range = [-10, 50];
  public $salt = null;
  public $key = null;
  public $fonts_path = null;

  // Initialize from global configuration.
  public function initialize() {
    if (defined('GF_TEXT_CAPTCHA_LENGTH')) {
      $length = intval(GF_TEXT_CAPTCHA_LENGTH);
      if ($length > 0) {
        $this->length = $length;
      }
    }

    if (defined('GF_TEXT_CAPTCHA_FONT')) {
      $this->font = GF_TEXT_CAPTCHA_FONT;
    }

    if (defined('GF_TEXT_CAPTCHA_FIGLET_ARGS')) {
      $this->figlet_args = GF_TEXT_CAPTCHA_FIGLET_ARGS;
    }

    if (defined('GF_TEXT_CAPTCHA_ALLOWED_CHARS')) {
      $this->allowed_chars = GF_TEXT_CAPTCHA_ALLOWED_CHARS;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_COUNT')) {
      $this->noise_count = intval(GF_TEXT_CAPTCHA_NOISE_COUNT);
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_COLORS')) {
      $this->noise_colors = explode(',', GF_TEXT_CAPTCHA_NOISE_COLORS);
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_OPACITY_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_OPACITY_RANGE), 0, 2);
      $range = array_filter($range, intval);
      $this->noise_opacity_range = $range;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_WIDTH_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_WIDTH_RANGE), 0, 2);
      $range = array_map('intval', $range);
      $this->noise_width_range = $range;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_HEIGHT_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_HEIGHT_RANGE), 0, 2);
      $range = array_map('intval', $range);
      $this->noise_height_range = $range;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_X_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_X_RANGE), 0, 2);
      $range = array_map('intval', $range);
      $this->noise_x_range = $range;
    }

    if (defined('GF_TEXT_CAPTCHA_NOISE_Y_RANGE')) {
      $range = array_slice(explode(',', GF_TEXT_CAPTCHA_NOISE_Y_RANGE), 0, 2);
      $range = array_map('intval', $range);
      $this->noise_y_range = $range;
    }

    $this->salt = AUTH_SALT;
    $this->key = AUTH_KEY;
    $this->fonts_path = dirname(__FILE__) . '/../fonts';
  }
}
