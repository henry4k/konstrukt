Conventions
===========


Naming
------

Use `CamelCase` for global/static definitions.  
Use `mixedCase` for local/member definitions.  
Use `CAPITAL_LETTERS` for constants.

File names should resemble the class/module/etc they contain.  
File names should be `CamelCase`.

Use `snake_case` for directories.


### C/C++

Use the `static` attribute for file-local definitions.  
Use the `const` attribute whenever possible.


### Lua

Begin names with an underscore to mark them as private.  
Like `_myPrivateAttribute`.

Package names resemble the file path that refers to the modue.

Classes created using middleclass start with the package name, but
omit the file extension. (You wouldn't postfix C++ classes either, right?)

    local Spark = class('core/effects/Spark', Effect)


Intra file organization
-----------------------

Statements should be ordered by their importance in a file:

1. Includes/Imports
2. Constants and Declarations
3. Definitions

Separate them by using 2 empty lines.


### C/C++

Hide as much implementation details as possible!  
E.g. don't declare structures in the header, but forward-declare them:
    struct MeshBuffer;
    MeshBuffer* CreateMeshBuffer();
    FreeMeshBuffer( MeshBuffer* buffer );
You can then be sure, that the only way to modify the MeshBuffer is to
*use the modules functions*.  
That makes testing alot easier.

An exception to this rule are performance critical parts:
Structures that are *created in huge amounts* and are probably *accessed very often*.


Documentation
-------------

Use JavaDoc and markdown to document source code.

Use `#GlobalDefinition`, neither `::GlobalDefinition` nor `GlobalDefinition()`!
If you can't refer to something using JavaDoc syntax,
use code formatting (backticks) to highlight it.

Example:

    /**
     * Summary goes here.
     *
     * More detailed description goes here
     * and may span multiple lines.
     *
     * @return
     * `aaa` or `0` if `aaa` is negative.
     */



---------------------------------


    Example/
        FluidTank/
            Init.lua
            Diffuse.png
            Scene.json

    example/
        fluid_tank/
            init.lua
            diffuse.png
            scene.json

    example/
        fluid_tank/
            Init.lua
            Diffuse.png
            Scene.json
