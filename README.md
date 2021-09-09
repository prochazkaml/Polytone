# Polytone
A modern re-implementation of the MONOTONE tracker with many enhancements.

![Polytone screenshot](assets/scrot.png)

Thanks to SDL2, it runs on all major platforms (Linux, macOS and Windows).
It uses its own file format (.POL) for expanded effect values & pattern packing,
but it still has the ability to import your old Monotone files.

New features include:
- Full-range effects (8-bit values instead of only 6-bit)!
- Pattern packing (so the file sizes are _usually_ smaller)
- Mouse support for navigating menus
- Cut/Copy/Paste (works system-wide, so you can easily copy parts of one song to another)
- Built-in fully documented effects reference (todo: a full manual)
- Colorful tracker window
- etc.

Please visit the [PTPlayer repo](https://github.com/prochazkaml/PTPlayer) for more information about MONOTONE and the Polytone file format.

## Building for Linux/macOS

``` bash
sudo apt-get install build-essential git libsdl2-dev # For Debian/Ubuntu
brew install sdl2                                    # For macOS
git clone --recurse-submodules https://github.com/prochazkaml/Polytone
cd Polytone
make
```

It's as simple as that. On macOS, make sure to install [Homebrew](https://brew.sh/) first.

## Cross-building for Windows (from Linux)

Firstly, it is necessary to install the prerequisites:

``` bash
sudo apt-get install build-essential git gcc-mingw-w64-x86-64 # For Debian/Ubuntu
git clone --recurse-submodules https://github.com/prochazkaml/Polytone
cd Polytone
```

Then you'll need to download the [SDL2 development library for MinGW](https://www.libsdl.org/download-2.0.php).
Extract it and navigate to the folder from where you can see the following:

(todo: add an image)

Make sure you are in the 64-bit directory (`x86_64-w64-mingw32`, not `i686-w64-mingw32`).
Copy the contents of the `include` and `lib` folders into the respective folders in `/usr/x86_64-w64-mingw32`.
Finally, copy `SDL2.dll` from the `bin` folder into the root of this repository.
Then you can start the build:

``` bash
make polytone.exe
```
