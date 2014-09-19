#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test/mock/Mock'

describe('Control')
    :setup(function()
        EventCallbacks = {}

        NATIVE = {
            RegisterKeyControl = Mock(),
            RegisterAxisControl = Mock(),
            SetEventCallback = function( name, fn )
                EventCallbacks[name] = fn
            end
        }

        FakeRequire:whitelist('core/Control')
        FakeRequire:install()

        Control = require 'core/Control'
    end)

    :beforeEach(function()
        NATIVE.RegisterKeyControl:reset()
        NATIVE.RegisterAxisControl:reset()
    end)

    :it('can register key controls.', function()
        NATIVE.RegisterKeyControl:whenCalledWith{'key control name'}
        Control.registerKey('key control name', 'key control callback')
        NATIVE.RegisterKeyControl:assertCallCount(1)
    end)

    :it('reacts to key events.', function()
        local keyControlCallback = Mock()
        keyControlCallback:whenCalledWith{true}
        NATIVE.RegisterKeyControl:whenCalledWith{'key control name', keyControllCallback}

        Control.registerKey('key control name', keyControlCallback)
        EventCallbacks['KeyControlAction']('key control name', true)

        keyControlCallback:assertCallCount(1)
    end)

    :it('can register axis controls.', function()
        NATIVE.RegisterAxisControl:whenCalledWith{'axis control name'}
        Control.registerAxis('axis control name', 'axis control callback')
        NATIVE.RegisterAxisControl:assertCallCount(1)
    end)

    :it('can register axis controls.', function()
        local axisControlCallback = Mock()
        axisControlCallback:whenCalledWith{1.0, 0.5}
        NATIVE.RegisterAxisControl:whenCalledWith{'axis control name', axisControllCallback}

        Control.registerAxis('axis control name', axisControlCallback)
        EventCallbacks['AxisControlAction']('axis control name', 1.0, 0.5)

        axisControlCallback:assertCallCount(1)
    end)


bdd.runTests()
