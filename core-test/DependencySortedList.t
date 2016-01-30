#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core-test/common'

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

    :it('raises an error for duplicated entries.', function()
        local dsl = DSL()
        dsl:add('a')
        assert(pcall(dsl.add, dsl, 'a') == false)
    end)

    :it('raises an error for missing dependencies.', function()
        local dsl = DSL()
        dsl:add('a', {'b'})
        assert(pcall(dsl.sort, dsl) == false)
    end)

    :it('raises an error for direct dependency cycles.', function()
        local dsl = DSL()
        dsl:add('a', {'b'})
        dsl:add('b', {'a'})
        assert(pcall(dsl.sort, dsl) == false)
    end)

    :it('raises an error for indirect dependency cycles.', function()
        local dsl = DSL()
        dsl:add('a', {'b'})
        dsl:add('b', {'c'})
        dsl:add('c', {'a'})
        assert(pcall(dsl.sort, dsl) == false)
    end)


bdd.runTests()
