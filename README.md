maim
====

maim (Make Image) is a utility that takes screenshots of your desktop using imlib2. It's meant to replace scrot and performs better than scrot and import in several ways.

features
--------
* Allows you to take a screenshot of your desktop and save it in any format.
* Allows you to take a screenshot of a predetermined region or window of your desktop.
* If slop (https://github.com/naelstrof/slop) is installed, it can be used for selecting a region to screenshot.

![slopgood](http://farmpolice.com/content/images/2014-10-14-12:14:51.png)
* Allows you to blend the system cursor to screenshots. (Why don't any other screenshooters do this?)

![screenshot with cursor](http://farmpolice.com/content/images/wow.png)

* Allows you to mask off-screen pixels to be black and transparent in screenshots. (Great for people who use an uneven multi-monitor setup!)

![screenshot mask comparison](http://farmpolice.com/content/images/mask_compare.png)

why use maim over import or scrot?
--------------------
* Compared to scrot
    - maim has no --exec or naming features. This is because maim follows the unix philosophy of "do one thing and do it well". These features are things that should be handled by the shell.
    - scrot has no way to screenshot a predefined region. maim comes equipped with --geometry features that allow for specified region capture.
    - With [slop](https://github.com/naelstrof/slop) installed, maim's --select option is far superior to scrot's -s option in many ways. See [slop](https://github.com/naelstrof/slop) for more details.
    - maim will never error with `giblib error: couldn't grab keyboard:Resource temporarily unavailable` as it never grabs the keyboard. (slop does, but it has proper error handling that keeps it from crashing.)
* Compared to ImageMagick's import
    - import doesn't play nicely with compositors; making effects like transparent windows not render properly in the screenshot. maim, like scrot, uses imlib2 which isn't inflicted with this problem.
* Compared to either
    - maim can actually take screenshots with your cursor included in them! It does this using the XFixes extension. I don't think there's any other screenshooters that do this.
    - For those of you with multiple monitors, maim is aware of which pixels are visible or not and will make off-screen pixels that are in screenshots black and transparent. Import and scrot both mindlessly include off-screen pixel data in their screenshots which is very often just garbage.

examples
-------------------
I'm including this section because some people don't see how powerful and flexible their shell can be with simple tools like maim. Remember you can always bind keys to shell commands!
The following can be executed in any bash-like shells:

* Set the screenshot's name to the current time and date:
```bash
$ maim "~/Pictures/$(date +%F-%T).png"
```

* Take a screenshot of the active window: (Requires xdotool.)
```bash
$ maim -id=$(xdotool getactivewindow)
```

* Custom transparent red selection with 10 pixel padding: (Requires [slop](https://github.com/naelstrof/slop).)
```bash
$ maim -s -c=1,0,0,0.6 -p=10
```
![Image of maim selecting a window](http://farmpolice.com/content/images/window_selection.png)

* Automatically upload selected region to Imgur: (Requires [Bart's Bash Script Imgur Uploader](http://imgur.com/tools/imgurbash.sh), xclip is optional.)
```bash
$ maim -s /tmp/screenshot.png; imgurbash.sh /tmp/screenshot.png
$ # If xclip is installed, your clipboard should have the online screenshot's URL in it!
```

In review, maim does one thing and does it well: it takes a screenshot of what you want. :) What you want is up to you, your programming skills, and your imagination.

help
-------------------
```text
Usage: maim [options] [file]
maim (make image) makes an image out of the given area on the given X screen. Defaults to the whole screen.

options
    -h, --help                         Show this message.
    -s, --select                       Manually select screenshot location. Only works when slop is installed.
    -xd=STRING, --xdisplay=STRING      Set x display (STRING must be hostname:number.screen_number format)
    -g=GEOMETRY, --geometry=GEOMETRY   Set the region to capture. GEOMETRY is in format WxH+X+Y
    -x=INT                             Set the x coordinate for taking an image
    -y=INT                             Set the y coordinate for taking an image
    -w=INT                             Set the width for taking an image
    -h=INT                             Set the height for taking an image
    -d=FLOAT, --delay=FLOAT            Set the amount of time to wait before taking an image.
    -id=INT, --windowid=INT            Set the window id to take a picture of, defaults to the root window id.
    -hc, --hidecursor                  Prevent the system cursor from appearing in the screenshot.
    -m=TYPE, --mask=TYPE               Mask screenshots so non-visible pixels are black and transparent.
                                           TYPE can be one of the following: ON, OFF, AUTO
                                           The AUTO setting only masks screenshots when they cover
                                           a majority of the screen and they aren't a specific window screenshot.
                                           --mask is set to AUTO by default.
    -v, --version                      Prints version.

slop options
    -nkb, --nokeyboard                 Disables the ability to cancel selections with the keyboard.
    -b=INT, --bordersize=INT           Set selection rectangle border size.
    -p=INT, --padding=INT              Set padding size for selection.
    -t=INT, --tolerance=INT            How far in pixels the mouse can move after clicking and still be detected
                                       as a normal click. Setting to zero will disable window selections.
    -c=COLOR, --color=COLOR            Set selection rectangle color, COLOR is in format FLOAT,FLOAT,FLOAT,FLOAT.
                                       takes RGBA or RGB.
    -w=FLOAT, --gracetime=FLOAT        Set the amount of time before slop will check for keyboard cancellations
                                       in seconds.
    -nd, --nodecorations               Attempts to remove decorations from window selections.
    -min=INT, --minimumsize=INT        Sets the minimum output of width or height values, useful to avoid outputting 0
                                       widths or heights.
    -max=INT, --maximumsize=INT        Sets the maximum output of width or height values.
    -hi, --highlight                   Instead of outlining the selection, slop highlights it. Only useful when
                                       used with a --color with an alpha under 1.
examples
    $ # Screenshot the active window
    $ maim -id=$(xdotool getactivewindow)

    $ # Prompt a transparent red selection to screenshot.
    $ maim -s -c=1,0,0,0.6

    $ # Save a dated screenshot.
    $ maim "~/$(date +%F-%T).png"
```
