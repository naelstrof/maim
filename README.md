# maim

A WIP remake of maim that uses cimg instead of imlib2.

- [x] Convert X11 formats to a ARGB format.
- [x] Link with slop library.
- [x] Option parsing.
- [x] Smartly constraining selection to prevent crashing.
- [ ] Masking off-screen buffers.
- [x] Man-pages.
- [x] Actual std::cout support.
- [x] PNG support
- [x] JPEG support
- [ ] Cursor blending
- [ ] ? support

To install:
```sh
git clone https://github.com/naelstrof/slop.git
cd slop
git checkout slop-release-candidate
cmake -DCMAKE_INSTALL_PREFIX="/usr" ./
sudo make install
cd ..
git clone https://github.com/naelstrof/maim.git
cd maim
git checkout maim-release-candidate
cmake -DCMAKE_INSTALL_PREFIX="/usr" ./
sudo make install
```
