# maim

maim (Make Image) is a utility that takes screenshots of your desktop using imlib2. It's meant to overcome shortcomings of scrot and performs better in several ways.

## Features
* Allows you to take a screenshot of your desktop and save it in any format.
* Allows you to take a screenshot of a predetermined region or window of your desktop.
* If slop (https://github.com/naelstrof/slop) is installed, it can be used for selecting a region to screenshot.

![slopgood](http://farmpolice.com/content/images/2014-10-14-12:14:51.png)
* Allows you to blend the system cursor to screenshots. (Why don't any other commandline screenshooters do this?)

![screenshot with cursor](http://farmpolice.com/content/images/wow.png)

* Allows you to mask off-screen pixels to be black and transparent in screenshots. (Great for people who use an uneven multi-monitor setup!)

![screenshot mask comparison](http://farmpolice.com/content/images/mask_compare2.png)

## Why use maim over import or scrot?
* Compared to scrot
    - maim has no --exec or naming features. This is because maim follows the unix philosophy of "do one thing and do it well". These features are things that should be handled by the shell.
    - scrot has no way to screenshot a predefined region. maim comes equipped with --geometry features that allow for specified region capture.
    - With [slop](https://github.com/naelstrof/slop) installed, maim's --select option is far superior to scrot's -s option in many ways. See [slop](https://github.com/naelstrof/slop) for more details.
    - maim will never error with `giblib error: couldn't grab keyboard:Resource temporarily unavailable` as it never grabs the keyboard. (slop does, but it has proper error handling that keeps it from crashing.)
* Compared to ImageMagick's import
    - import doesn't play nicely with compositors; making effects like transparent windows not render properly in the screenshot. maim, like scrot, uses imlib2 which isn't inflicted with this problem.
* Compared to either
    - maim can actually take screenshots with your cursor included in them! It does this using the XFixes extension. To my knowledge, no other commandline screenshot utility does this.
    - For those of you with multiple monitors, maim is aware of which pixels are visible or not and will make off-screen pixels that are in screenshots black and transparent. Import and scrot both mindlessly include off-screen pixel data in their screenshots which is very often just garbage.

## Examples
I'm including this section because some people don't see how powerful and flexible their shell can be with simple tools like maim. Remember you can always bind keys to shell commands!
The following can be executed in any bash-like shells:

* Set the screenshot's name to the current time and date:
```bash
$ maim ~/Pictures/$(date +%F-%T).png
```

* Take a screenshot of the active window: (Requires xdotool.)
```bash
$ maim -i $(xdotool getactivewindow)
```

* Custom transparent red selection with 10 pixel padding: (Requires [slop](https://github.com/naelstrof/slop).)
```bash
$ maim -s -c 1,0,0,0.6 -p 10
```
![Image of maim selecting a window](http://farmpolice.com/content/images/window_selection.png)

* Automatically upload selected region to Imgur: (Requires [Bart's Bash Script Imgur Uploader](http://imgur.com/tools/imgurbash.sh), xclip is optional.)
```bash
$ maim -s /tmp/screenshot.png; imgurbash.sh /tmp/screenshot.png
$ # If xclip is installed, your clipboard should have the online screenshot's URL in it!
```

In review, maim does one thing and does it well: it takes a screenshot of what you want. :) What you want is up to you, your programming skills, and your imagination.

## How to install

### Install using your Package Manager (preferred)

* [Arch Linux: community/maim](https://www.archlinux.org/packages/community/x86_64/maim/)
* [Debian: maim](https://tracker.debian.org/pkg/maim)
* [Void Linux: maim](https://github.com/voidlinux/void-packages/blob/24ac22af44018e2598047e5ef7fd3522efa79db5/srcpkgs/maim/template)
* [FreeBSD: graphics/maim](http://www.freshports.org/graphics/maim/)
* [OpenBSD: graphics/maim](http://openports.se/graphics/maim)
* [CRUX: 6c37/maim](https://github.com/6c37/crux-ports/tree/3.2/maim)
* [Gentoo: media-gfx/maim::fkmclane](https://github.com/fkmclane/overlay/tree/master/media-gfx/maim)
* Please make a package for maim on your favorite system, and make a pull request to add it to this list.


### Install using CMake (Requires CMake)

Note: Dependencies should be installed first: Imlib2, libXrandr, and libXfixes.

```bash
git clone https://github.com/naelstrof/maim.git
cd maim
cmake ./
make && sudo make install
```

Make sure to check out and install [slop](https://github.com/naelstrof/slop) too if you want selection capabilities!

Special Thanks for Special People
----
[Evan Purkhiser](https://github.com/EvanPurkhiser)

Help
----
Join us on irc at freenode in *#maim*.
```text
maim v3.4.47

Copyright (C) 2014 Dalton Nell, Maim Contributors
(https://github.com/naelstrof/maim/graphs/contributors)

Takes screenshots.

Usage: maim [options] [file]

maim (Make Image) is a utility that takes screenshots of your desktop using
imlib2. It's meant to overcome shortcomings of scrot and performs better than
scrot in several ways.

      --help                    Print help and exit
  -V, --version                 Print version and exit
Options
      --xdisplay=hostname:number.screen_number
                                Sets the x display.
      --format=FORMAT           Sets output format (png, jpg, etc)
                                  (default=`auto')
  -s, --select                  Enables user region selection. Requires slop to
                                  be installed.  (default=off)
  -x, --x=INT                   Sets the x coordinate for taking an image
  -y, --y=INT                   Sets the y coordinate for taking an image
  -w, --w=INT                   Sets the width for taking an image
  -h, --h=INT                   Sets the height for taking an image
  -g, --geometry=WxH+X+Y        Set the region to capture
  -d, --delay=FLOAT             Set the amount of time to wait before taking an
                                  image.  (default=`0.0')
  -i, --windowid=INT            Set the window to capture. Defaults to the root
                                  window id.
      --localize                Localizes given geometry to the given window.
                                  So "maim -i $ID -g 100x100+0+0 --localize"
                                  would screenshot the top-left 100x100 pixels
                                  of the given window, rather than the top-left
                                  100x100 pixels of the root window.
                                  (default=off)
      --showcursor              Causes the system cursor to be blended on top
                                  of the screenshot.  (default=off)
      --hidecursor              Deprecated, does nothing.  (default=off)
  -m, --mask=STRING             Masks off-screen pixels so they don't show up
                                  in screenshots.  (possible values="auto",
                                  "off", "on" default=`auto')

Slop Options
      --nokeyboard              Disables the ability to cancel selections with
                                  the keyboard.  (default=off)
  -b, --bordersize=INT          Set the selection rectangle's thickness. Does
                                  nothing when --highlight is enabled.
                                  (default=`5')
  -p, --padding=INT             Set the padding size of the selection. Can be
                                  negative.  (default=`0')
  -t, --tolerance=INT           How far in pixels the mouse can move after
                                  clicking and still be detected as a normal
                                  click instead of a click and drag. Setting
                                  this to 0 will disable window selections.
                                  (default=`2')
      --gracetime=FLOAT         Set the amount of time before slop will check
                                  for keyboard cancellations in seconds.
                                  (default=`0.4')
  -c, --color=FLOAT,FLOAT,FLOAT,FLOAT
                                Set the selection rectangle's color. Supports
                                  RGB or RGBA values.
                                  (default=`0.5,0.5,0.5,1')
  -n, --nodecorations           Attempt to select child windows in order to
                                  avoid window decorations.  (default=off)
      --min=INT                 Set the minimum output of width or height
                                  values. This is useful to avoid outputting 0.
                                  Setting min and max to the same value
                                  disables drag selections.  (default=`0')
      --max=INT                 Set the maximum output of width or height
                                  values. Setting min and max to the same value
                                  disables drag selections.  (default=`0')
  -l, --highlight               Instead of outlining selections, slop
                                  highlights it. This is only useful when
                                  --color is set to a transparent color.
                                  (default=off)
      --opengl                  Enable hardware acceleration. Only works with
                                  modern systems that are also running a
                                  compositor.  (default=off)
      --magnify                 Display a magnifying glass when --opengl is
                                  also enabled.  (default=off)
      --magstrength=FLOAT       Sets how many times the magnification window
                                  size is multiplied.  (default=`4')
      --magpixels=INT           Sets how many pixels are displayed in the
                                  magnification. The less pixels the bigger the
                                  magnification.  (default=`64')
      --theme=STRING            Sets the theme of the selection, using textures
                                  from ~/.config/slop/ or /usr/share/.
                                  (default=`none')
      --shader=STRING           Sets the shader to load and use from
                                  ~/.config/slop/ or /usr/share/.
                                  (default=`simple')

Examples
    $ # Screenshot the active window
    $ maim -i $(xdotool getactivewindow)

    $ # Prompt a transparent red selection to screenshot.
    $ maim -s -c 1,0,0,0.6

    $ # Save a dated screenshot.
    $ maim ~/$(date +%F-%T).png

    $ # Output screenshot to stdout.
    $ maim --format png /dev/stdout
```
