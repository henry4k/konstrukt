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

Package names resemble the file path that refers to the module:
`core/welder/WelderEffects` refers to `core/welder/WelderEffects.lua`.

Classes created using middleclass,
must contain the complete package path in their name.

    local Spark = class('core/effects/Spark', Effect)


Intra file organization
-----------------------

Statements should be ordered by their importance in a file:

1. Includes/Imports
2. Constants and Declarations
3. Definitions

Separate them by using 2 empty lines.


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

