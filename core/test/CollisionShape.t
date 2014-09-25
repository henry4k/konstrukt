#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test.mock.Mock'


describe('A collision shape')
    :setup(function()
        ENGINE = {
            DestroyCollisionShape = Mock()
        }

        FakeRequire:whitelist('core/CollisionShape')
        FakeRequire:whitelist('core/middleclass')
        FakeRequire:install()

        CollisionShape = require 'core/CollisionShape'
    end)

    :beforeEach(function()
        ENGINE.DestroyCollisionShape:reset()
    end)

    :it('can be created and destroyed.', function()
        ENGINE.DestroyCollisionShape:canBeCalled{with={'collision shape handle'}}

        local cs = CollisionShape('collision shape handle')
        assert(cs.handle == 'collision shape handle')

        cs:destroy()
        assert(cs.handle == nil)
        ENGINE.DestroyCollisionShape:assertCallCount(1)
    end)


bdd.runTests()
