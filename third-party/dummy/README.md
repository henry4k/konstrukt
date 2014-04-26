dummy
=====

A flexible test framework for C.


synopsis
--------

At its core, it just consists of a small set of functions, written in C.
Since small things tend to break less often than big contraptions.
And a test system is not the kind of code that may break.

    [API porcelain]
          ||
          \/
        [core]
          ||
          \/
      [reporter]

Reporters and API porcelain can be plugged onto it, just as you need it.
Some are already provided, but it shouldn't be hard
to write your own reporter or porcelain.


tl;dr
-----

- small core
- no external dependencies
- portable, only uses the standard library
- extensible: reporters and API porcelain
- small binaries


licence
-------

*TODO*
But probably a very permissive licence, that allows you to copy and modify
the code to your needs.
(MIT,BSD,zlib .. ?)

Henry Kielmann
