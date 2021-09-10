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
3. In WordPress admin, find plugin "Gravity Forms Text CAPTCHA" and activate.

# How to Use
1. When building a form, find field "Text CAPTCHA" in the "Advanced" section.
2. Add the Text CAPTCHA field.
3. Enjoy little to no spam.
