Apoapsis
========

A game about the solitude and survival in a high-tech lump 400 thousand meters
above the earth.

The project is still very WIP!

This repository contains the engine and the core package.
... which is pretty useless for itself.  Take a look at the example package
at https://github.com/henry4k/apoapsis-example to see how it can be used.


## Dependencies

Runtime dependencies:

- [PhysFS 2.0+](http://icculus.org/physfs/)
- Lua 5.2
- Open GL
- [GLFW 3](http://www.glfw.org/)
- Open AL
- ALURE
- [Bullet](http://bulletphysics.org/)


Compile time depencencies:

- [tup](http://gittup.org/tup/) (the build system)
- [GLM](http://glm.g-truc.net/)
- Python 3 (needed by flextGL)
- Python 3 package wheezy.template (needed by flextGL)


Ideally install pkg-config and use one of the provided sample
tup.config files.


## Compiling

After preparing the runtime and compiletime dependencies, you need to tell tup
where it can find these dependencies and which compilation flags it should
use.

For this you must create a file called `tup.config` in the project root.
See the provided sample configs for reference.

If the compiler complains about a missing `OnDebugEvent` function, then add
`-DAPOAPSIS_GL_DEBUG_FIX` to the `CFLAGS` in your config.
The problem occurs, when the OpenGL `ARB_debug_output` extension defines its
callback syntax with a slightly different parameter set.  If you know why this
happens, it would be great if you could send me a short note. :)


## Running the tests

The tests use the [Test Anything Protocol](http://testanything.org/).  A TAP
compatible test runner like `prove` can be used to run the test suite.

The tests are located in `engine/tests` and `core-test`.


## License and copyright

Copyright © Henry Kielmann

Apoapsis is licensed under the MIT license, which can be found in the
`LICENSE` file.

The project comes bundled with a number of dependencies, each with its own license.
See their respective readme and license files in the `third-party` folder
for details.
