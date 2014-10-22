Abstract
--------

Packages are directories or archives, which contain scripts and resources.


Mounting
--------

Packages can be mounted and unmounted in the virtual file system.
Mounted packages are accessible under its basename.  This also prevents that a
package is being mounted multiple times.

E.g. the package `example` is stored on disk under `/usr/share/apoapsis/packages/example.zip`.
Once mounted, its contents are available under `example/...`.


Search paths
------------

The physical locations of packages are resolved using a list of search paths,
separated by semicolons.

For example if the search paths look like that:
`/home/nick/.apoapsis/packages;/usr/share/apoapsis/packages` and the user wants
to mount a package called `example`, the following paths are tried:

1. `/home/nick/.apoapsis/packages/example`
2. `/home/nick/.apoapsis/packages/example.zip`
3. `/usr/share/apoapsis/packages/example`
4. `/usr/share/apoapsis/packages/example.zip`

The first path that exists, is then mounted.


Meta data
---------

A package must contain a meta data file called `meta.json` located in the
packages root directory.  It describes the package, e.g. which dependencies it has:

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
3. Initialize each package by running all init scripts it contains.
4. Run the scenarios start script.


Scenarios
---------

Packages that setup the actual scene, which the player can interact with,
are called scenarios.  They must be marked in `meta.json` as such and contain a
start script called `start.lua` in the packages root directory.

The start script is called after all packages has been initialized.

Note:  Only the first package passed as start argument to Apoapsis, can be used
as scenario.