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
maim's design philosophy is "do one thing and do it well". Unlike scrot, maim doesn't have fancy options like --delay or --exec. It also doesn't have any advanced naming capabilities either, and this is by design. Your shell is supposed to take care of these features, a delay can easily be done with
```bash
$ sleep 5; maim
```
If you want your files to have timestamps for names, your shell is capable of that too!
```bash
$ maim "~/Pictures/$(date +%D-%r).png"
```


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
