lua-bdd4tap
===========

A BDD wrapper for the [lua-testmore](http://fperrad.github.io/lua-TestMore/).


```lua
#!/usr/bin/env lua
require 'Test.More'
local bdd = require 'test/bdd'
local describe = require 'test/bdd/describe'

describe('fizzbuzzer')
    :it('does things', function()
        -- try doing things here
    end)
    :it('is a slacker', function()
        todo('Implement lazyness')
        -- prove that fizzbuzzer is lazy as fuck
    end)
    :it('can\'t handle bloops', function()
        todo('Implement exception')
        -- test if bloops cause fizzbuzzer to fail
        error('Exception not implemented yet')
    end)

bdd.runTests()
```


Author
------

- [Henry Kielmann](http://henry4k.de/)


Licence
-------

This is free and unencumbered public domain software.  
For more information, see http://unlicense.org/ or the accompanying `UNLICENSE` file.
