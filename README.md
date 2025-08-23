# maim
maim (Make Image) is an utility that takes screenshots of your desktop. It's meant to overcome shortcomings of scrot and performs better in several ways.

## Features
* Takes screenshots of your desktop, and saves it in png, jpg, or bmp format.
* Takes screenshots of predetermined regions or windows, useful for automation.
* Allows a user to select a region, or window, before taking a screenshot on the fly.

![screenshot with selection](http://i.imgur.com/ILZKJCT.png)
* Blends the system cursor to the screenshot.
![screenshot with cursor](http://i.imgur.com/PD1bgBg.png)
* Masks off-screen pixels to be transparent or black.

![screenshot with masked pixels](http://i.imgur.com/kMkcHlZ.png)
* Maim cleanly pipes screenshots directly to standard output (unless otherwise specified), allowing for command chaining.
* Maim supports anything slop does, even selection [shaders](https://github.com/naelstrof/slop#shaders)!

![slop animation](http://i.giphy.com/kfBLafeJfLs2Y.gif)


## Installation

### Install using your Package Manager (Preferred)
* [ALT Linux: maim](https://packages.altlinux.org/ru/sisyphus/srpms/maim)
* [Arch Linux: extra/maim](https://www.archlinux.org/packages/extra/x86_64/maim/)
* [Debian: maim](https://tracker.debian.org/pkg/maim)
* [Ubuntu: maim](https://packages.ubuntu.com/search?keywords=maim)
* [Void Linux: maim](https://github.com/void-linux/void-packages/tree/master/srcpkgs/maim/template)
* [FreeBSD: graphics/maim](http://www.freshports.org/graphics/maim/)
* [NetBSD: x11/maim](http://pkgsrc.se/x11/maim)
* [OpenBSD: graphics/maim](http://openports.se/graphics/maim)
* [CRUX: maim](https://crux.nu/portdb/?a=search&q=maim)
* [Gentoo: media-gfx/maim](https://packages.gentoo.org/packages/media-gfx/maim)
* [NixOS: maim](https://github.com/NixOS/nixpkgs/blob/master/pkgs/tools/graphics/maim/default.nix)
* [GNU Guix: maim](https://packages.guix.gnu.org/packages/maim/)
* [Ravenports: maim](http://www.ravenports.com/catalog/bucket_B4/maim/standard/)
* [Fedora: maim](https://src.fedoraproject.org/rpms/maim)
* [Fedora EPEL (RHEL, CentOS): maim](https://src.fedoraproject.org/rpms/maim)
* [Alpine Linux: maim](https://pkgs.alpinelinux.org/packages?name=maim&branch=edge&repo=&arch=&maintainer=)
* Please make a package for maim on your favorite system, and make a pull request to add it to this list.

### Install using CMake
#### Requires these at build-time:
CMake, glm, x11proto

#### Requires these at build- and runtime:
libjpeg-turbo, libpng, libwebp, libX11, libXcomposite, libXext, libXfixes, libXrandr, libXrender
```bash
git clone https://github.com/naelstrof/slop.git
cd slop
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="/usr" ./
make && sudo make install
cd ..
git clone https://github.com/naelstrof/maim.git
cd maim
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="/usr" ./
make && sudo make install
```

## Examples
Maim allows for a lot of unique and interesting functionalities. Here's an example of a few interactions.

* This command will allow you to select an area on your screen, then copy the selection to your clipboard. This can be used to easily post images in mumble, discord, gimp-- or any other image supporting application.
```bash
$ maim -s | xclip -selection clipboard -t image/png
```

* This messy command forces a user to select a window to screenshot, then applies a shadow effect using *imagemagick*, then saves it to shadow.png. It looks really nice on windows that support an alpha channel.
```bash
$ maim -st 9999999 | convert - \( +clone -background black -shadow 80x3+5+5 \) +swap -background none -layers merge +repage shadow.png
```

* This command is a particular favorite of mine, invented by a friend. It simply prints the RGB values of the selected pixel. A basic color picker that has the additional ability to average out the pixel values of an area. If used cleverly with the geometry and window flag, the return color might warn you of a found counter-strike match...
```bash
$ maim -st 0 | convert - -resize 1x1\! -format '%[pixel:p{0,0}]' info:-
```

* This is a basic, but useful command that simply screenshots the current active window.
```bash
$ maim -i $(xdotool getactivewindow) ~/mypicture.jpg
```

* This is another basic command, but I find it necessary to describe the usefulness of date. This particular command creates a full screenshot, and names it as the number of seconds that passed since 1970. Guaranteed unique, already sorted, and easily read.
```bash
$ maim ~/Pictures/$(date +%s).png
```

* This one overlays a still of your desktop, then allows you to crop it. Doesn't play well with multiple monitors, but I'm sure if it did it wouldn't look this pretty and simple.
```bash
$  maim -u | feh -F - & maim -s -k cropped.png && kill $!
```

* Finally with the [help your friendly neighborhood scripter](https://github.com/tremby/imgur.sh), pictures can automatically be uploaded and their URLs copied to the clipboard with this basic command.
```bash
$ maim -s /tmp/screenshot.png; imgur.sh /tmp/screenshot.png | xclip -selection clipboard
```

* The following command can be used to select a QR code (or click into a window
  where a QR code is present), decode it, print the text to the console and
  copy the text into the clipboard for further usage. 

```bash
$ maim -qs | zbarimg -q --raw - | xclip -selection clipboard -f
```

* Shortcut for [i3 window manager](https://github.com/i3/i3):
  * Enable light on the Thinkpad T430 laptop
  * Wait 5 seconds
  * Screenshot all displays
  * Save file like **~/screenshots/2022-dec-21--12-56-08_maim.png**
  * Disable light
  * Show i3 notification for 3 seconds:

```bash
bindsym $mod+Shift+x exec "\
    echo 1 > /sys/class/leds/platform\:\:micmute/brightness; \
    sleep 5; \
    maim --hidecursor ~/screenshots/$(date +%Y-%b-%d--%H-%M-%S_maim | tr A-Z a-z).png; \
    echo 0 > /sys/class/leds/platform\:\:micmute/brightness; \
    i3-nagbar --message 'Screenshot created' --type warning & \
    sleep 3; pkill i3-nagbar"
```
* Shortcut for I3 to take a cropped screenshot and save it with current date in ISO 8601 date followed by a custom filename.

```bash
bindsym $mod+Shift+s exec --no-startup-id \
  "outfilePrefix=$(dmenu -p 'Enter output filename without prefix or spaces' < /dev/null) && \
   maim -s --hidecursor ~/screenshots/$(date +%Y-%m-%d_)$outfilePrefix.png || \
   i3-nagbar --type warning --message 'Error: Maim exited with non-zero exit code'"
```
