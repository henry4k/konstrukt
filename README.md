Konstrukt
=========

![](icons/blueprint_square.svg)

![](icons/blueprint_circle.svg)

A 3D game engine.

The project is still very WIP!

This repository contains the engine and the core package.
... which is pretty useless for itself.  Take a look at the example package
at https://github.com/henry4k/apoapsis-example to see how it can be used.


## Running the engine

The engine has the following command line interface:

`konstrukt [options] <scenario package> <other packages ...>`


### Options

#### `--state=...` (optional)

Path to a directory in which the simulation state can be stored.
Without this option the engine will create a temporary directory.


#### `--shared-state=...` (optional)

Path to a directory which stores data, that is shared by all/multiple
scenarios.  Without this option the engine will create a temporary directory.


#### `-I...` (optional)

Add a package search path.  That is a directory, which is used to resolve
package file names.  The engines default search path is always tried first.


#### `--config=...` (optional)

Add config values by parsing the given [INI file](https://en.wikipedia.org/wiki/INI_file).


#### `-D<key>=<value>` (optional)

Set a config value.


### Packages

Packages must be passed either using their base name or file path.

A packages base name looks like this: `<name>.<major>.<minor>.<patch>` (e. g. `example.1.2.3`)

If a package is passed using its base name, the engine uses the search paths
to resolve its file path.

The first package is used as scenario.


## Dependencies

Runtime dependencies:

- [PhysFS 2.0+](http://icculus.org/physfs/)
- Lua 5.2
- Open GL 3.2
- [GLFW 3](http://www.glfw.org/)
- Open AL
- ALURE
- [Bullet](http://bulletphysics.org/)


Compile time depencencies:

- CMake
- Python 3 (needed by flextGL)
- Python 3 package wheezy.template (needed by flextGL)


## Issues

If the compiler complains about a missing `OnDebugEvent` function, then enable
the `KONSTRUKT_GL_DEBUG_FIX` in CMake.  The problem occurs, when the OpenGL
`ARB_debug_output` extension defines its callback syntax with a slightly
different parameter set.  If you know why this happens, it would be great if
you could send me a short note. :)


## Running the tests

The tests use the [Test Anything Protocol](http://testanything.org/).  A TAP
compatible test runner like `prove` can be used to run the test suite.

The tests are located in `engine/tests` and `core-test`.


## License and copyright

Copyright © Henry Kielmann

Konstrukt is licensed under the MIT license, which can be found in the
`LICENSE` file.

The project comes bundled with a number of dependencies, each with its own license.
See their respective readme and license files in the `third-party` folder
for details.
