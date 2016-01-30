Abstract
--------

Packages are directories or archives, which contain scripts and resources.


Mounting
--------

Packages can be mounted and unmounted in the virtual file system.
Mounted packages are accessible under their basename.  This also prevents that a
package is being mounted multiple times.

E.g. the package `example` is stored on disk under `/usr/share/konstrukt/packages/example.zip`.
Once mounted, its contents are available under `example/...`.


Search paths
------------

The physical locations of packages are resolved using a list of search paths,
separated by semicolons.

For instance if the list looks like that:
`/home/nick/.konstrukt/packages;/usr/share/apoapsis/packages` and the user wants
to mount a package called `example`, the following paths are tried:

1. `/home/nick/.konstrukt/packages/example`
2. `/home/nick/.konstrukt/packages/example.zip`
3. `/usr/share/konstrukt/packages/example`
4. `/usr/share/konstrukt/packages/example.zip`

The first path that exists is mounted.


Meta data
---------

A package must contain a meta data file called `meta.json` located in the
packages root directory.  It describes the package:

- `type`:
At the moment there are only `regular` and `scenario` packages.
Defaults to `regular`.

- `dependencies`:
A list of packages which this package depends on.

It could for example look like this:
```json
{
    "type": "scenario",

    "dependencies":
    [
        "advanced-lasers",
        "fluid-compressors",
        "more-heat-radiators"
    ]
}
```


Initialization
--------------

When starting a game, packages are loaded and initialized this way:

1. Mount all packages and their dependencies.
2. Work out correct initialization order using the dependency information.
3. Initialize each package by loading all Lua modules it contains.
4. Call the `start` function in the scenarios main module. (See below.)


Scenarios
---------

Packages that setup the actual scene, which the player can interact with,
are called scenarios.  They must be marked in `meta.json` as such and contain a
main module called `init.lua`, which must export a function called `start`.

The start function is called after all packages have been initialized.
It could be called like this:  `require scenarioName..'/init'.start()`

Note:  Only the first package passed as start argument to Konstrukt, can be used
as scenario.
