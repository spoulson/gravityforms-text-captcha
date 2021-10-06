# Gravity Forms Text CAPTCHA
Because Google reCAPTCHA sucks.

I've had it up to here --> * <--- with spam making it through Gravity Forms
even with reCAPTCHA enabled.  I figure a custom text-based CAPTCHA is less
likely to be defeated by script kiddies.

This is a custom field for Gravity Forms that renders a Figlet text CAPTCHA
easily readable by a human.

For example:

```
            ooooooooo.   oooooooooo.                           .oooo.
            `888   `Y88. `888'   `Y8b                        .dP""Y88b
oooo    ooo  888   .d88'  888     888 oooo    ooo  .ooooo oo       ]8P'
 `88.  .8'   888ooo88P'   888oooo888'  `88.  .8'  d88' `888      .d8P'
  `88..8'    888          888    `88b   `88..8'   888   888    .dP'
   `888'     888          888    .88P    `888'    888   888  .oP     .o
    .8'     o888o        o888bood8P'      .8'     `V8bod888  8888888888
.o..P'                                .o..P'            888.
`Y8P'                                 `Y8P'             8P'
                                                        "
```

# Features
* Easy to use out of the box.
* High configurability.
* Text rendered by Figlet.  Can use provided Roman font or add/create your own.
* Easy to style with CSS.
* "Noise" shapes rendered randomly overtop the CAPTCHA text in various shapes
  and colors to make OCR text recognition and manual entry more difficult.

# System Requirements
* WordPress 5.8 or newer.
* Gravity Forms 2.5 or newer.
* Figlet installed on web server.
   * For example, on Debian-based systems: `apt install figlet`
   * Test figlet with command such as `figlet Test` to render "Test" in ASCII art.

# Installation from Source
1. Run `make build` to build the file:
   `gravityforms-text-captcha-<version>.tar.gz`, where `<version>` corresponds
   to the branch name that is checked out.
2. Untar this file into WordPress directory: `wp-content/plugins`.
3. In WordPress admin, activate plugin "Gravity Forms Text CAPTCHA".

# How to Use
1. When building a form, add field "Text CAPTCHA" from the "Advanced" section.
2. Enjoy little to no spam.

# Configuration
Global constants set in wp-config.php:

Constant                        | Default         | Description
------------------------------- | --------------- | --------------------------------------
`GF_TEXT_CAPTCHA_ALLOWED_CHARS` | *alphanumerics* | Allowed characters for CAPTCHA string.
`GF_TEXT_CAPTCHA_FIGLET_ARGS`   | `-w 1000`       | Additional arguments to Figlet.
`GF_TEXT_CAPTCHA_FONT`          | `roman`         | Figlet font name.
`GF_TEXT_CAPTCHA_LENGTH`        | 6               | CAPTCHA character length.
`GF_TEXT_CAPTCHA_NOISE_COLORS`  | `#0A67A5,#E98F01,#C31D25,#E4E6EC,#E1C591` | Noise color scheme containing 1 or more colors, comma separated.
`GF_TEXT_CAPTCHA_NOISE_COUNT`   | 5               | Number of noisy shapes rendered on the CAPTCHA.
`GF_TEXT_CAPTCHA_NOISE_HEIGHT_RANGE` | `20,200`   | Inclusive range of noise height in px, comma separated.
`GF_TEXT_CAPTCHA_NOISE_OPACITY_RANGE` | `30,80`   | Inclusive range of noise opacity in percent, comma separated.
`GF_TEXT_CAPTCHA_NOISE_WIDTH_RANGE` | `20,200`    | Inclusive range of noise width in px, comma separated.
`GF_TEXT_CAPTCHA_NOISE_X_RANGE` | `-10,70`        | Inclusive range of noise X offset in percent, comma separated.
`GF_TEXT_CAPTCHA_NOISE_Y_RANGE` | `-10,50`        | Inclusive range of noise Y offset in percent, comma separated.

Add additional fonts by copying font files (with `flf` file extension) to the
`fonts` directory and setting `GF_TEXT_CAPTCHA_FONT` to the filename without
extension.

To RTFM on Figlet and find more fonts, see http://www.figlet.org.

# Developer Setup
1. Install Composer: https://getcomposer.org
2. Run `composer install`.

# Source Verification
```
$ make lint test
```

Or run in container:
```
$ docker compose run --rm src lint test
```
