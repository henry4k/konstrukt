#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test/mock/Mock'


describe('A force')
    :setup(function()
        NATIVE = {
            CreateForce = Mock(),
            DestroyForce = Mock(),
            SetForce = Mock()
        }

        FakeRequire:whitelist('core/Force')
        FakeRequire:whitelist('core/middleclass')
        FakeRequire:whitelist('core/Vector')
        FakeRequire:install()

        Vec = require 'core/Vector'
        Force = require 'core/Force'
    end)

    :beforeEach(function()
        NATIVE.CreateForce:reset()
        NATIVE.DestroyForce:reset()
        NATIVE.SetForce:reset()
    end)

    :it('can be created and destroyed.', function()
        NATIVE.CreateForce:canBeCalled{with={'solid handle'}, thenReturn={'force handle'}}
        NATIVE.DestroyForce:canBeCalled{with={'force handle'}}

        local force = Force('solid handle')
        assert(force.handle == 'force handle')
        NATIVE.CreateForce:assertCallCount(1)

        force:destroy()
        assert(force.handle == nil)
        NATIVE.DestroyForce:assertCallCount(1)
    end)

    :it('can change its properties.', function()
        NATIVE.CreateForce:canBeCalled{with={'solid handle'}, thenReturn={'force handle'}}
        NATIVE.SetForce:canBeCalled{with={'force handle',
                                          1, 2, 3,
                                          10, 20, 30,
                                          true}}

        local force = Force('solid handle')
        force:set(Vec(1,2,3), Vec(10, 20, 30), true)

        NATIVE.CreateForce:assertCallCount(1)
        NATIVE.SetForce:assertCallCount(1)
    end)


bdd.runTests()
