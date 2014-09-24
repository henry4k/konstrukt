#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'apoapsis.core.test.common'

local Vector = require 'apoapsis.core.Vector'


describe('A vector')
    :it('can be created.', function()
        local v = Vector(42, 43)
        assert(#v == 2)
        assert(v[1] == 42)
        assert(v[2] == 43)
    end)

    :it('can be validated.', function()
        local v = Vector(1, 2)
        assert(Vector:isInstance(v)  == true)
        assert(Vector:isInstance({}) == false)
        assert(Vector:isInstance(42) == false)
    end)

    :it('returns 0 for unassigned fields.', function()
        local v = Vector(42, 43)
        assert(v[3] == 0)
        assert(v[99] == 0)
    end)

    :it('can be unpacked.', function()
        local v = Vector(42, 43)

        local x = v:unpack(1)
        assert(x == 42)

        local x,y = v:unpack(2)
        assert(x == 42)
        assert(y == 43)

        local x,y,z = v:unpack(3)
        assert(x == 42)
        assert(y == 43)
        assert(z == 0)
    end)

    :it('can be tested for equality.', function()
        local a = Vector(42, 43)
        local b = Vector(42, 43)
        local c = Vector(98, 99)
        assert(a == b)
        assert(a ~= c)
    end)

    :it('can use arithmetic operations with other vectors.', function()
        local a = Vector(1, 2)
        local b = Vector(3, 4)
        local r = a + b
        assert(r[1] == 4)
        assert(r[2] == 6)
    end)


    :it('can use arithmetic operations with vectors of different size.', function()
        local a = Vector(1, 1)
        local b = Vector(1, 1, 1)
        local r = a + b
        assert(r[1] == 2)
        assert(r[2] == 2)
        assert(r[3] == 1)
    end)

    :it('can be multiplied/divided with numbers.', function()
        local v = Vector(1, 2)
        local r = v * 3
        assert(r[1] == 3)
        assert(r[2] == 6)
    end)

    :it('can be negated.', function()
        local v = Vector(1, -2)
        local r = -v
        assert(r[1] == -1)
        assert(r[2] == 2)
    end)

    :it('can be converted to a string.', function()
        local v = Vector(1, 2)
        assert(tostring(v) == '1,2')
    end)


bdd.runTests()
