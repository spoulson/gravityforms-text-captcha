<?php declare(strict_types=1);
use PHPUnit\Framework\TestCase;

// Stub GF_Field class.
class GF_Field {
  function __construct($data = []) {
  }
}

final class GF_Field_TextCaptchaTest extends TestCase {
  public function setUp(): void {
    GF_Field_TextCaptcha::$salt = 'FoobarSalt';
    GF_Field_TextCaptcha::$key = 'FoobarKey';
    GF_Field_TextCaptcha::$fonts_path = dirname(__FILE__) . '/../../assets/fonts';
  }

  public function testCanConfigureCaptchaLength(): void {
    $args = [
      'GF_TEXT_CAPTCHA_LENGTH' => 123
    ];
    $field = new GF_Field_TextCaptcha($args);
    $this->assertEquals(123, $field->length);
  }

  public function testCanConfigureFont(): void {
    $args = [
      'GF_TEXT_CAPTCHA_FONT' => 'FoobarFont'
    ];
    $field = new GF_Field_TextCaptcha($args);
    $this->assertEquals('FoobarFont', $field->font);
  }

  public function testCanConfigureFigletArgs(): void {
    $args = [
      'GF_TEXT_CAPTCHA_FIGLET_ARGS' => 'FoobarArgs'
    ];
    $field = new GF_Field_TextCaptcha($args);
    $this->assertEquals('FoobarArgs', $field->figlet_args);
  }

  public function testCanConfigureAllowedCharacters(): void {
    $args = [
      'GF_TEXT_CAPTCHA_ALLOWED_CHARS' => 'FoobarAllowedChars'
    ];
    $field = new GF_Field_TextCaptcha($args);
    $this->assertEquals('FoobarAllowedChars', $field->allowed_chars);
  }

  public function testCanGenerateCaptchaCode(): void {
    $class = new ReflectionClass('GF_Field_TextCaptcha');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $field = new GF_Field_TextCaptcha();
    $captcha_str = 'FoobarCaptcha';

    // Generate code.
    $code = $generate_captcha_code->invokeArgs($field, [$captcha_str]);

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($field, [$captcha_str, $code]);
    $this->assertTrue($result);
  }

  public function testRejectsFaultyCaptchaCode(): void {
    $class = new ReflectionClass('GF_Field_TextCaptcha');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $field = new GF_Field_TextCaptcha();
    $captcha_str = 'FoobarCaptcha';
    $code = 'Bogus';

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($field, [$captcha_str, $code]);
    $this->assertFalse($result);
  }

  public function testRejectsIncorrectCaptchaString(): void {
    $class = new ReflectionClass('GF_Field_TextCaptcha');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $field = new GF_Field_TextCaptcha();
    $captcha_str = 'FoobarCaptcha';

    // Generate code.
    $code = $generate_captcha_code->invokeArgs($field, [$captcha_str]);

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($field, ['Bogus', $code]);
    $this->assertFalse($result);
  }

  // Make sure generated CAPTCHA code is encrypted with common salt used as AES
  // initialization vector.
  public function testSaltedCaptchaCode(): void {
    $class = new ReflectionClass('GF_Field_TextCaptcha');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $field = new GF_Field_TextCaptcha();
    $captcha_str = 'FoobarCaptcha';

    // Generate code.
    $code = $generate_captcha_code->invokeArgs($field, [$captcha_str]);

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($field, [$captcha_str, $code]);
    $this->assertTrue($result);

    // Verify using different salt, expecting failure.
    $field2 = new GF_Field_TextCaptcha();
    GF_Field_TextCaptcha::$salt = 'Bogus';
    $result = $verify_captcha->invokeArgs($field2, [$captcha_str, $code]);
    $this->assertFalse($result);
  }

  // Make sure generated CAPTCHA code is encrypted using a common key.
  public function testKeyedCaptchaCode(): void {
    $class = new ReflectionClass('GF_Field_TextCaptcha');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $field = new GF_Field_TextCaptcha();
    $captcha_str = 'FoobarCaptcha';

    // Generate code.
    $code = $generate_captcha_code->invokeArgs($field, [$captcha_str]);

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($field, [$captcha_str, $code]);
    $this->assertTrue($result);

    // Verify using different key, expecting failure.
    $field2 = new GF_Field_TextCaptcha();
    GF_Field_TextCaptcha::$key = 'Bogus';
    $result = $verify_captcha->invokeArgs($field2, [$captcha_str, $code]);
    $this->assertFalse($result);
  }

  public function testGenerateCaptchaString(): void {
    $class = new ReflectionClass('GF_Field_TextCaptcha');
    $generate_captcha_str = $class->getMethod('generate_captcha_str');
    $generate_captcha_str->setAccessible(true);
    $field = new GF_Field_TextCaptcha();
    $field->length = 6;

    // Generate string.
    $captcha_str = $generate_captcha_str->invoke($field);

    // Verify.
    $this->assertEquals(6, strlen($captcha_str));
  }

  // Requires figlet be installed.
  public function testCanRenderFiglet(): void {
    $class = new ReflectionClass('GF_Field_TextCaptcha');
    $make_figlet_image = $class->getMethod('make_figlet_image');
    $make_figlet_image->setAccessible(true);
    $field = new GF_Field_TextCaptcha();
    $field->fonts_path = dirname(__FILE__) . '/../../assets/fonts';
    $captcha_str = 'Foobar';

    // Generate string.
    $result = $make_figlet_image->invokeArgs($field, [$captcha_str]);

    // Verify.
    $this->assertNotEmpty($result);
  }
}
