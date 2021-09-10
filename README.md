# Gravity Forms Text CAPTCHA
Because Google reCAPTCHA sucks.

I've had it up to here --> * <--- with spam making it through Gravity Forms
even with reCAPTCHA enabled.  A custom text-based CAPTCHA is less likely to be
defeated by script kiddies.

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
* You must be running Gravity Forms 2.5 or newer.
* Figlet installed on web server.
   * For example on Debian-based systems:
```
$ apt install figlet
```
   * Test figlet with command such as `figlet Test` to render "Test" in ASCII art.

# Manual Installation
1. Create directory in WordPress: `wp-content/plugins/gravityforms-text-captcha`.
2. Copy `gravityforms-text-captcha.php` and `fonts` to that directory.
3. In WordPress admin, activate plugin "Gravity Forms Text CAPTCHA".

# How to Use
1. When building a form, add field "Text CAPTCHA" from the "Advanced" section.
2. Enjoy little to no spam.

# Configuration
Global constants set in wp-config.php:

Constant                      | Default   | Description
----------------------------- | --------- | -------------------------------
`GF_TEXT_CAPTCHA_FIGLET_ARGS` | `-w 1000` | Additional arguments to Figlet.
`GF_TEXT_CAPTCHA_FONT`        | `roman`   | Figlet font name.
`GF_TEXT_CAPTCHA_LENGTH`      | 6         | CAPTCHA character length.

Add additional fonts by copying font files (with `flf` file extension) to the
`fonts` directory and setting `GF_TEXT_CAPTCHA_FONT` to the filename without
extension.

More Figlet fonts can be found at: http://www.figlet.org.
