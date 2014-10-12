maim
====

maim (Make Image) is a utility that takes screenshots of your desktop using imlib2. It's meant to replace scrot and performs better than scrot and import in several ways.

features
--------
* Allows you to take a screenshot of your desktop and save it in any format.
* Allows you to take a screenshot of a predetermined region of your desktop and save it in any format.
* If slop (https://github.com/naelstrof/slop) is installed, it can be used for selecting a region.

why use maim over import or scrot?
--------------------
* Compared to scrot
    - maim has no --exec or naming features. This is because maim follows the unix philosophy of "do one thing and do it well". These features are things that should be handled by your shell.
    - scrot has no way to screenshot a predefined region. maim comes equipped with --geometry features that allow for specified region capture.
    - With [slop](https://github.com/naelstrof/slop) installed, maim's --select option is far superior to scrot's -s option in many ways. See [slop](https://github.com/naelstrof/slop) for more details.
* Compared to ImageMagick's import
    - import doesn't play nicely with compositors; making effects like transparent windows not render properly in the screenshot. maim, like scrot, uses imlib2 which isn't inflicted with this problem.

examples
-------------------
I'm including this section because some people don't see how powerful and flexible their shell can be with simple tools like maim. Remember you can always bind keys to shell commands!
The following can be executed in any bash-like shells:

* Set the screenshot's name to the current time and date:
```bash
$ maim "~/Pictures/$(date +%F-%T).png"
```

* Take a screenshot of the active window: (Requires xwininfo, xdotool, and awk.)
```bash
$ maim -g=$(xwininfo -id $(xdotool getactivewindow) | awk '/geometry/ {print $2}')
$ # Note: Some programs misreport their actual pixel geometry (mostly terminals).
$ # This can be fixed by using their absolute X/Y/Width/Height values instead;
$ # which is much more difficult and confusing to parse in one line from xwininfo,
$ # and as such won't be included in this example.
```

* Custom transparent red selection with 10 pixel padding: (Requires [slop](https://github.com/naelstrof/slop))
```bash
$ maim -s -c=1,0,0,0.6 -p=10
```
![Image of maim selecting a window](http://farmpolice.com/content/images/window_selection.png)

* Automatically upload selected region to Imgur: (Requires [Bart's Bash Script Imgur Uploader](http://imgur.com/tools/imgurbash.sh), xclip is optional)
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
    -h, --help                         show this message.
    -s, --select                       manually select screenshot location. Only works when slop is installed.
    -d=STRING, --display=STRING        set x display (STRING must be hostname:number.screen_number format)
    -g=GEOMETRY, --geometry=GEOMETRY   set the region to capture. GEOMETRY is in format WxH+X+Y
    -x=INT                             set the x coordinate for taking an image
    -y=INT                             set the y coordinate for taking an image
    -w=INT                             set the width for taking an image
    -h=INT                             set the height for taking an image
    -v, --version                      prints version.

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
    $ # Screenshot the entire screen besides the top 30 pixels.
    $ maim -g=1920x1060+0+30

    $ # Prompt a selection to screenshot.
    $ maim -s

    $ # Wait 5 seconds before saving a screenshot to ~/delayed.png
    $ sleep 5; maim ~/delayed.png

    $ # Save a dated screenshot.
    $ maim "~/$(date +%F-%T).gif"

    $ # Save a .jpg
    $ maim ~/myscreen.jpg
```
