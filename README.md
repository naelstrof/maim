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
scrot's selection option (-s) is quite bad. It draws the selection rectangle directly to the root window which causes some gross things to happen. It also fails to run if it doesn't succeed in grabbing the keyboard. Finally scrot doesn't have any options to easily capture a predefined region of the screen. Otherwise scrot works great, it's just the -s option that has poor support.

import works great in every way besides lacking a selection option and being incapable of taking screenshots of transparent windows.

maim not only has a proper selection option (thanks to slop), but it also lets you disable keyboard grabbing. And like scrot it uses imlib2 to take the screenshots so you don't have to worry about transparent windows rendering improperly in the images you take.
