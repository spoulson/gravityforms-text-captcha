<?php

class GF_TextCaptcha_API {
  private $generator;

  function __construct($cfg) {
    $this->generator = new GF_TextCaptcha_Generator($cfg);
  }

  // Call register_routes() in the rest_api_init action.
  public function register_routes() {
    register_rest_route('gravityforms-text-captcha/v1', '/form/(?P<id>\d+)/captcha', [
      'methods' => 'GET',
      'callback' => [$this, 'get_captcha'],
      'args' => [
        'id' => [
          'validate_callback' => function ($param, $request, $key) {
            return is_numeric($param);
          }
        ]
      ]
    ]);
  }

  public function get_captcha(WP_REST_Request $request) {
    $form_id = (int) $request->get_param('id');
    $is_editmode = $request->get_param('editmode') != 'false';
    return $this->generator->generate_html($form_id, $is_editmode);
  }
}
