#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core-test/common'

local Mock = require 'test.mock.Mock'
local class = require 'middleclass'

describe('Control')
    :setup(function()
        EventCallbacks = {}

        ENGINE = {
            RegisterControl = Mock(),
            SetEventCallback = function( name, fn )
                EventCallbacks[name] = fn
            end
        }

        FakeRequire:whitelist('middleclass')
        FakeRequire:whitelist('core/Control')
        FakeRequire:whitelist('core/Controllable')
        FakeRequire:install()

        Control = require 'core/Control'
        Controllable = require 'core/Controllable'

        DummyControllable = class('DummyControllable')
        DummyControllable:include(Controllable)
        function DummyControllable:initialize()
            self:initializeControllable()
        end
        function DummyControllable:destroy()
            self:destroyControllable()
        end
        DummyControllableCallback = Mock()
        ENGINE.RegisterControl:canBeCalled{with={'do-something'}}
        DummyControllable:mapControl('do-something', DummyControllableCallback)
    end)

    :beforeEach(function()
        ENGINE.RegisterControl:reset()
        DummyControllableCallback:reset()
        Control.controllableStack = {}
    end)

    :it('can register controls.', function()
        ENGINE.RegisterControl:canBeCalled{with={'control name'}}
        Control.register('control name')
        ENGINE.RegisterControl:assertCallCount(1)
    end)

    :it('can push and pop controllables.', function()
        local controllable = DummyControllable()
        Control.pushControllable(controllable)
        assert(#Control.controllableStack == 1)
        Control.popControllable(controllable)
        assert(#Control.controllableStack == 0)
    end)

    :it('notifies pushed controllables about events.', function()
        controllable = DummyControllable()

        Control.pushControllable(controllable)
        DummyControllableCallback:assertCallCount(0)

        DummyControllableCallback:canBeCalled{with={controllable, 1, 1}}
        EventCallbacks['ControlAction']('do-something', 1, 1)
        DummyControllableCallback:assertCallCount(1)
    end)

    :it('won\'t notify popped controllables about events anymore.', function()
        controllable = DummyControllable()
        Control.pushControllable(controllable)
        Control.popControllable(controllable)
        EventCallbacks['ControlAction']('do-something', 1, 1)
        DummyControllableCallback:assertCallCount(0)
    end)


bdd.runTests()
