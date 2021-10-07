<?php declare(strict_types=1);
require('GF_TextCaptcha_Config.inc');
require('GF_TextCaptcha_Generator.inc');

use \PHPUnit\Framework\TestCase;

function makeTestCfg() {
  $cfg = new GF_TextCaptcha_Config();
  $cfg->salt = 'FoobarSalt';
  $cfg->key = 'FoobarKey';
  $cfg->fonts_path = dirname(__FILE__) . '/../../assets/fonts';
  return $cfg;
}

final class GF_TextCaptcha_GeneratorTest extends TestCase {
  private $cfg;

  protected function setUp(): void {
    $this->cfg = makeTestCfg();
  }

  public function testCanGenerateCaptchaCode(): void {
    $class = new ReflectionClass('GF_TextCaptcha_Generator');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $generator = new GF_TextCaptcha_Generator($this->cfg);
    $captcha_str = 'FoobarCaptcha';

    // Generate code.
    $code = $generate_captcha_code->invokeArgs($generator, [$captcha_str]);

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($generator, [$captcha_str, $code]);
    $this->assertTrue($result);
  }

  public function testRejectsFaultyCaptchaCode(): void {
    $class = new ReflectionClass('GF_TextCaptcha_Generator');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $generator = new GF_TextCaptcha_Generator($this->cfg);
    $captcha_str = 'FoobarCaptcha';
    $code = 'Bogus';

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($generator, [$captcha_str, $code]);
    $this->assertFalse($result);
  }

  public function testRejectsIncorrectCaptchaString(): void {
    $class = new ReflectionClass('GF_TextCaptcha_Generator');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $generator = new GF_TextCaptcha_Generator($this->cfg);
    $captcha_str = 'FoobarCaptcha';

    // Generate code.
    $code = $generate_captcha_code->invokeArgs($generator, [$captcha_str]);

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($generator, ['Bogus', $code]);
    $this->assertFalse($result);
  }

  // Make sure generated CAPTCHA code is encrypted with common salt used as AES
  // initialization vector.
  public function testSaltedCaptchaCode(): void {
    $class = new ReflectionClass('GF_TextCaptcha_Generator');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $generator = new GF_TextCaptcha_Generator($this->cfg);
    $captcha_str = 'FoobarCaptcha';

    // Generate code.
    $code = $generate_captcha_code->invokeArgs($generator, [$captcha_str]);

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($generator, [$captcha_str, $code]);
    $this->assertTrue($result);

    // Verify using different salt, expecting failure.
    $cfg2 = makeTestCfg();
    $cfg2->salt = 'Bogus';
    $generator2 = new GF_TextCaptcha_Generator($cfg2);
    $result = $verify_captcha->invokeArgs($generator2, [$captcha_str, $code]);
    $this->assertFalse($result);
  }

  // Make sure generated CAPTCHA code is encrypted using a common key.
  public function testKeyedCaptchaCode(): void {
    $class = new ReflectionClass('GF_TextCaptcha_Generator');
    $generate_captcha_code = $class->getMethod('generate_captcha_code');
    $generate_captcha_code->setAccessible(true);
    $generator = new GF_TextCaptcha_Generator($this->cfg);
    $captcha_str = 'FoobarCaptcha';

    // Generate code.
    $code = $generate_captcha_code->invokeArgs($generator, [$captcha_str]);

    // Verify code.
    $verify_captcha = $class->getMethod('verify_captcha');
    $verify_captcha->setAccessible(true);
    $result = $verify_captcha->invokeArgs($generator, [$captcha_str, $code]);
    $this->assertTrue($result);

    // Verify using different key, expecting failure.
    $cfg2 = makeTestCfg();
    $cfg2->key = 'Bogus';
    $generator2 = new GF_TextCaptcha_Generator($cfg2);
    $result = $verify_captcha->invokeArgs($generator2, [$captcha_str, $code]);
    $this->assertFalse($result);
  }

  public function testGenerateCaptchaString(): void {
    $class = new ReflectionClass('GF_TextCaptcha_Generator');
    $generate_captcha_str = $class->getMethod('generate_captcha_str');
    $generate_captcha_str->setAccessible(true);
    $generator = new GF_TextCaptcha_Generator($this->cfg);

    // Generate string.
    $captcha_str = $generate_captcha_str->invoke($generator);

    // Verify.
    $this->assertEquals($this->cfg->length, strlen($captcha_str));
  }

  // Requires figlet be installed.
  public function testCanRenderFiglet(): void {
    $class = new ReflectionClass('GF_TextCaptcha_Generator');
    $make_figlet_image = $class->getMethod('make_figlet_image');
    $make_figlet_image->setAccessible(true);
    $generator = new GF_TextCaptcha_Generator($this->cfg);
    $captcha_str = 'Foobar';

    // Generate string.
    $result = $make_figlet_image->invokeArgs($generator, [$captcha_str]);

    // Verify.
    $this->assertNotEmpty($result);
  }

  public function testScramblesCaptchaLines(): void {
    $class = new ReflectionClass('GF_TextCaptcha_Generator');
    $html_format_captcha_text = $class->getMethod('html_format_captcha_text');
    $html_format_captcha_text->setAccessible(true);
    $generator = new GF_TextCaptcha_Generator($this->cfg);
    $captcha_str = 'Foobar';
    $captcha_text = "AAA\nBBB\nCCC\nDDD\n";

    // Call code.
    $result = $html_format_captcha_text->invokeArgs($generator, [$captcha_text]);

    // Verify results are shuffled.
    $unexpected = implode("\n", [
      '<ul class="gfield_test_captcha_str">',
      '<li style="order: 0">AAA</li>',
      '<li style="order: 1">BBB</li>',
      '<li style="order: 2">CCC</li>',
      '<li style="order: 3">DDD</li>',
      '</ul>'
    ]);
    $this->assertNotEquals($unexpected, $result);
  }
}
