<?php

// REST API endpoints for use by Text CAPTCHA field.
class GF_TextCaptcha_API {
  private $generator;

  function __construct($cfg) {
    $this->generator = new GF_TextCaptcha_Generator($cfg);
  }

  // Call register_routes() in the rest_api_init action.
  public function register_routes() {
    register_rest_route('gravityforms-text-captcha/v1', '/form/(?P<form_id>\d+)/captcha/(?P<field_id>\d+)', [
      'methods' => 'GET',
      'callback' => [$this, 'get_captcha'],
      'args' => [
        'form_id' => [
          'validate_callback' => function ($param, $request, $key) {
            return is_numeric($param);
          }
        ],
        'field_id' => [
          'validate_callback' => function ($param, $request, $key) {
            return is_numeric($param);
          }
        ]
      ]
    ]);
  }

  // Generate new CAPTCHA HTML for Text CAPTCHA field contents.
  // @param {WP_REST_Request} $request
  // @return {any} JSON serializable response content.
  public function get_captcha(WP_REST_Request $request) {
    $field_id = (int) $request->get_param('field_id');
    $is_editmode = $request->get_param('editmode') != 'false';
    return [
      'html' => $this->generator->generate_html($field_id, $is_editmode)
    ];
  }
}
