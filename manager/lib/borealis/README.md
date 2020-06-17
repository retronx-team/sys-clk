# borealis

Hardware accelerated, Nintendo Switch inspired UI library for PC and Nintendo Switch. Powered by nanovg. Skeleton based on the [hybrid glfw app](https://github.com/fincs/hybrid_app) by fincs.

The library is still in early development and WILL lack features and contain bugs. Feel free to report them in the issues, or make a pull request if you happen to fix some.

You can see the planned features in the Projects tab.

Documentation is available [right here](https://github.com/natinusala/borealis/wiki). Warning: may not be up to date.

## Building the example for Switch

To build for Switch, a standard development environment must first be set up. In order to do so, [refer to the Getting Started guide](https://devkitpro.org/wiki/Getting_Started).

```bash
(sudo) (dkp-)pacman -S switch-glfw switch-mesa switch-glm
make -j
nxlink -s borealis_example.nro
```

## Building the example for PC

To build for PC, the following components are required:

- meson/ninja build system
- A C++ compiler supporting the C++17 standard
- GLFW version 3.3 or higher (as a static library)
- GLM version 0.9.8 or higher

Please refer to the usual sources of information for your particular operating system. Usually the commands needed to build this project will look like this:

```bash
meson build
ninja -C build
./build/borealis_example
```

Also, please note that the `resources` folder must be available in the working directory, otherwise the program will fail to find the shaders.

### Building the example for Windows using msys2

msys2 provides all packages needed to build this project:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-ninja mingw-w64-x86_64-pkg-config mingw-w64-x86_64-glfw mingw-w64-x86_64-glm
meson build
ninja -C build
./build/borealis_example
```

### Including in your project (TL;DR: see the example makefile in this repo)
0. Your project must be built as C++17 (`-std=c++1z`). You also need to remove `-fno-rtti` and `-fno-exceptions` if you have them
1. Use a submodule (or even better, a [subrepo](https://github.com/ingydotnet/git-subrepo)) to clone this repository in your project
2. Copy the `resources` folder to the root of your project
3. For PC (meson):
    1. take a standard meson file
    2. use `subdir` to import the library folder
    3. use the `borealis_files`, `borealis_dependencies` and `borealis_include` variables for respectively objects to build, dependencies (glfw...) and includes directory
    4. add a `BOREALIS_RESOURCES` define pointing to the resources folder at runtime (so `resources`)
4. For Switch:
    1. take a standard homebrew makefile
    2. add a `BOREALIS_PATH` variable containing the subfolder you put the library in
    3. use `include` to load `borealis.mk` (after `LIBDIRS` and `BOREALIS_PATH`)
    4. set `ROMFS` to the resources folder
    5. add a `BOREALIS_RESOURCES` define pointing to the resources folder at runtime (so `romfs:/`)
