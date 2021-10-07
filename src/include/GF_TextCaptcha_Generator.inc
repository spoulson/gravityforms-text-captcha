<?php
// Generate CAPTCHA HTML.
class GF_TextCaptcha_Generator {
  // GF_TextCaptcha_Config object.
  private $cfg;

  // $cfg: GF_TextCaptcha_Config object
  function __construct($cfg) {
    $this->cfg = $cfg;
  }

  // Generate text CAPTCHA field HTML.
  // $input_id: Form id.
  // $is_editmode: True if rendering in form editor.
  // Returns HTML.
  public function generate_html($input_id, $is_editmode) {
    $captcha_str = $this->generate_captcha_str();
    $code = $this->generate_captcha_code($captcha_str);

    $noise_html = $this->html_noise();

    $captcha_text = htmlentities($this->make_figlet_image($captcha_str));
    $captcha_html = $this->html_format_captcha_text($captcha_text);

    $input_attrs = "";
    if ($is_editmode) {
      $input_attrs = " disabled=\"disabled\"";
    }

    $input_html = <<<EOF
<div class="ginput_container ginput_container_text_captcha"">
<input type="text" id="input_${input_id}" name="input_${input_id}[]" class="large"${input_attrs} />
</div>
EOF;

    $code_hex = bin2hex($code);
    $hidden_html = <<<EOF
<input type="hidden" name="input_${input_id}[]" value="${code_hex}" class="gform_hidden" />
EOF;

    return <<<EOF
<div class="gfield_text_captcha_wrapper">
  ${noise_html}${captcha_html}
</div>
${input_html}${hidden_html}
EOF;
  }

  // Verify CAPTCHA string with code.
  // $code must be an unencoded binary string.
  // Returns true if CAPTCHA matches.
  public function verify_captcha($captcha_str, $code) {
    $secret = $this->cfg->key;
    $ciphertext = $code;
    $iv = hash_pbkdf2('sha1', $this->cfg->salt, 0, 1, 16, true);
    $key = hash_pbkdf2('sha1', $secret, 0, 1, 32, true);
    $plaintext = openssl_decrypt($ciphertext, 'aes-256-cbc', $key, OPENSSL_RAW_DATA, $iv);
    if ($plaintext === false) {
      error_log('verify_captcha() decrypt failed');
      return false;
    }
    $captcha_verify = substr($plaintext, 4);
    return $captcha_verify === $captcha_str;
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
    $colors = $this->cfg->noise_colors;
    $output = [];

    for ($i = 0; $i < $this->cfg->noise_count; $i++) {
      $opacity = random_int(...$this->cfg->noise_opacity_range) / 100;
      $width = random_int(...$this->cfg->noise_width_range); // px
      $height = random_int(...$this->cfg->noise_height_range); // px
      $left = random_int(...$this->cfg->noise_x_range); // %
      $top = rand(...$this->cfg->noise_y_range); // %
      $z_index = random_int(0, 1); // 0 = behind text, 1 = in front of text
      $color = $colors[array_rand($colors)];
      $rotation = random_int(0, 359);
      $shape = random_int(0, 2);

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
        // Triangle.
        switch (random_int(0, 1)) {
        case 0:
          // Isosceles triangle.
          $half_width = $width / 2;
          $output[] = <<<EOF
  <div style="position:absolute; width:0; height:0; border-left:${half_width}px solid transparent; border-right:${half_width}px solid transparent; border-bottom:${height}px solid ${color}; opacity:${opacity}; z-index:${z_index}; left:${left}%; top:${top}%; transform:rotate(${rotation}deg); transform-origin: 50% 50%"></div>
EOF;
          break;
        case 1:
          // Right triangle.
          $output[] = <<<EOF
  <div style="position:absolute; width:0; height:0; border-bottom: ${height}px solid ${color}; border-right: ${width}px solid transparent; opacity:${opacity}; z-index:${z_index}; left:${left}%; top:${top}%; transform:rotate(${rotation}deg); transform-origin: 50% 50%"></div>
EOF;
          break;
        }
        break;
      }
    }

    return implode("\n", $output);
  }

  // Return a newly generates CAPTCHA string.
  private function generate_captcha_str() {
    $captcha_str = '';
    $rand_max = strlen($this->cfg->allowed_chars) - 1;

    for ($i = 0; $i < $this->cfg->length; $i++) {
      $r = random_int(0, $rand_max);
      $captcha_str .= substr($this->cfg->allowed_chars, $r, 1);
    }

    return $captcha_str;
  }

  // Generate encrypted code from CAPTCHA string.
  // Returns an unencoded binary string.
  private function generate_captcha_code($captcha_str) {
    // TODO: Use HMAC instead.
    $secret = $this->cfg->key;
    $salt = openssl_random_pseudo_bytes(4);
    $iv = hash_pbkdf2('sha1', $this->cfg->salt, 0, 1, 16, true);
    $key = hash_pbkdf2('sha1', $secret, 0, 1, 32, true);
    $plaintext = $salt . $captcha_str;
    return openssl_encrypt($plaintext, 'aes-256-cbc', $key, OPENSSL_RAW_DATA, $iv);
  }

  // Render figlet text.
  // Convert input string to text-based ASCII art.
  private function make_figlet_image($str) {
    $cmd = sprintf('figlet -d %s %s -f %s %s',
      $this->cfg->fonts_path,
      $this->cfg->figlet_args,
      $this->cfg->font,
      escapeshellcmd($str)
    );
    // TODO: Handle error.
    return shell_exec($cmd);
  }
}