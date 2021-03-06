<?php
/*
Plugin Name: Gravity Forms Text CAPTCHA
Plugin URI:  https://github.com/spoulson/gravityforms-text-captcha
Description: Text-based CAPTCHA field for Gravity Forms.
Version:     0.1.0-alpha.7
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

if (class_exists('GF_Fields')) {
  $path = plugin_dir_path(__FILE__);
  require_once($path . 'include/GF_TextCaptcha_API.inc');
  require_once($path . 'include/GF_TextCaptcha_Config.inc');
  require_once($path . 'include/GF_Field_TextCaptcha.inc');

  // Register field in Gravity Forms.
  $field = new GF_Field_TextCaptcha();
  GF_Fields::register($field);

  // Add custom CSS.
  if (is_admin()) {
    // Admin page.
    add_action('admin_enqueue_scripts', function ($hook_suffix) {
      // Enqueue CSS if on edit form admin page.
      if ($hook_suffix == 'toplevel_page_gf_edit_forms') {
        $url = plugin_dir_url(__FILE__);
        wp_register_style('gravityforms-text-captcha', $url . 'admin_style.css');
        wp_enqueue_style('gravityforms-text-captcha');
      }
    });
  } else {
    // Normal page.
    // Register REST API endpoints.
    add_action('rest_api_init', function () {
      $cfg = new GF_TextCaptcha_Config();
      $api = new GF_TextCaptcha_API($cfg);
      $api->register_routes();
    });

    add_action('wp_enqueue_scripts', function () {
      $url = plugin_dir_url(__FILE__);
      wp_register_style('gravityforms-text-captcha', $url . 'style.css');
      wp_enqueue_style('gravityforms-text-captcha');
    });
  }
}
