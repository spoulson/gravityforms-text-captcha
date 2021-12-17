<?php
// Text CAPTCHA field configuration class.
class GF_TextCaptcha_Config {
  public $length = 6;
  public $width = 1000;
  public $font = 'roman';

  // Smush layout modes bitmask.
  // See: https://github.com/laminas/laminas-text/blob/8879e75d03e09b0d6787e6680cfa255afd4645a7/src/Figlet/Figlet.php#L21
  // Default (0)  = SM_KERN
  // -1           = No smush
  // SM_EQUAL     = 0x01
  // SM_LOWLINE   = 0x02
  // SM_HIERARCHY = 0x04
  // SM_PAIR      = 0x08
  // SM_BIGX      = 0x10
  // SM_HARDBLANK = 0x20
  // SM_KERN      = 0x40
  public $smush_mode = 0; // range [-1, 63]

  public $allowed_chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  public $noise_count = 5;
  public $noise_colors = ['#0A67A5', '#E98F01', '#C31D25', '#E4E6EC', '#E1C591'];
  public $noise_opacity_range = [30, 80];
  public $noise_width_range = [20, 200];
  public $noise_height_range = [20, 200];
  public $noise_x_range = [-10, 70];
  public $noise_y_range = [-10, 50];
  public $salt = '';
  public $key = '';
  public $fonts_path = null;
  public $font_filename = null;

  public function __construct() {
    // Initialize from global configuration.
    if (defined('GF_TEXT_CAPTCHA_LENGTH')) {
      $length = intval(GF_TEXT_CAPTCHA_LENGTH);
      if ($length > 0) {
        $this->length = $length;
      }
    }

    if (defined('GF_TEXT_CAPTCHA_FONT')) {
      $this->font = GF_TEXT_CAPTCHA_FONT;
    }

    if (defined('GF_TEXT_CAPTCHA_WIDTH')) {
      $this->width = intval(GF_TEXT_CAPTCHA_WIDTH);
    }

    if (defined('GF_TEXT_CAPTCHA_SMUSH_MODE')) {
      $this->smush_mode = intval(GF_TEXT_CAPTCHA_SMUSH_MODE);
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

    if (defined('AUTH_SALT')) {
      $this->salt = AUTH_SALT;
    }

    if (defined('AUTH_KEY')) {
      $this->key = AUTH_KEY;
    }

    $this->fonts_path = __DIR__ . '/../fonts';
    $this->font_filename = $this->fonts_path . "/" . $this->font . ".flf";
  }
}
