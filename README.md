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

# System Requirements
* WordPress 5.8 or newer.
* Gravity Forms 2.5 or newer.
* Figlet installed on web server.
   * For example on Debian-based systems: `apt install figlet`
   * Test figlet with command such as `figlet Test` to render "Test" in ASCII art.

# Manual Installation
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

Add additional fonts by copying font files (with `flf` file extension) to the
`fonts` directory and setting `GF_TEXT_CAPTCHA_FONT` to the filename without
extension.

To RTFM on Figlet and find more fonts, see http://www.figlet.org.

# Developer Setup
1. Install Composer: https://getcomposer.org
2. Run `composer install`.

# Run Unit Tests
```
$ make test
```

Or run in container:
```
$ docker compose run --rm src test
```
