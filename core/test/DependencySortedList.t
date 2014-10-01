#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local DSL = require 'core/DependencySortedList'


describe('A DSL')
    :it('can sort dependencies.', function()
        local dsl = DSL()
        dsl:add('a', {'b', 'c'})
        dsl:add('b', {'c'})
        dsl:add('c')
        local result = dsl:sort()
        assert(#result == 3)
        assert(result[1] == 'c')
        assert(result[2] == 'b')
        assert(result[3] == 'a')
    end)


bdd.runTests()
