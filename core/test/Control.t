#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test.mock.Mock'

describe('Control')
    :setup(function()
        EventCallbacks = {}

        ENGINE = {
            RegisterKeyControl = Mock(),
            RegisterAxisControl = Mock(),
            SetEventCallback = function( name, fn )
                EventCallbacks[name] = fn
            end
        }

        FakeRequire:whitelist('core/Control')
        FakeRequire:whitelist('core/Controlable')
        FakeRequire:install()

        Control = require 'core/Control'
    end)

    :beforeEach(function()
        ENGINE.RegisterKeyControl:reset()
        ENGINE.RegisterAxisControl:reset()
    end)

    :it('can register key controls.', function()
        ENGINE.RegisterKeyControl:canBeCalled{with={'key control name'}}
        Control.registerKey('key control name', 'key control callback')
        ENGINE.RegisterKeyControl:assertCallCount(1)
    end)

    :it('reacts to key events.', function()
        local keyControlCallback = Mock()
        keyControlCallback:canBeCalled{with={true}}
        ENGINE.RegisterKeyControl:canBeCalled{with={'key control name', keyControllCallback}}

        Control.registerKey('key control name', keyControlCallback)
        EventCallbacks['KeyControlAction']('key control name', true)

        keyControlCallback:assertCallCount(1)
    end)

    :it('can register axis controls.', function()
        ENGINE.RegisterAxisControl:canBeCalled{with={'axis control name'}}
        Control.registerAxis('axis control name', 'axis control callback')
        ENGINE.RegisterAxisControl:assertCallCount(1)
    end)

    :it('can register axis controls.', function()
        local axisControlCallback = Mock()
        axisControlCallback:canBeCalled{with={1.0, 0.5}}
        ENGINE.RegisterAxisControl:canBeCalled{with={'axis control name', axisControllCallback}}

        Control.registerAxis('axis control name', axisControlCallback)
        EventCallbacks['AxisControlAction']('axis control name', 1.0, 0.5)

        axisControlCallback:assertCallCount(1)
    end)


bdd.runTests()
