#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test/mock/Mock'


describe('A collision shape')
    :setup(function()
        NATIVE = {
            DestroyCollisionShape = Mock()
        }

        FakeRequire:whitelist('core/CollisionShape')
        FakeRequire:whitelist('core/middleclass')
        FakeRequire:install()

        CollisionShape = require 'core/CollisionShape'
    end)

    :beforeEach(function()
        NATIVE.DestroyCollisionShape:reset()
    end)

    :it('can be created and destroyed.', function()
        NATIVE.DestroyCollisionShape:canBeCalled{with={'collision shape handle'}}

        local cs = CollisionShape('collision shape handle')
        assert(cs.handle == 'collision shape handle')

        cs:destroy()
        assert(cs.handle == nil)
        NATIVE.DestroyCollisionShape:assertCallCount(1)
    end)


bdd.runTests()
