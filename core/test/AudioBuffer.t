#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'apoapsis.core.test.common'

local Mock = require 'test.mock.Mock'


describe('An audio buffer')
    :setup(function()
        NATIVE = {
            LoadAudioBuffer = Mock(),
            DestroyAudioBuffer = Mock()
        }

        FakeRequire:whitelist('apoapsis.core.AudioBuffer')
        FakeRequire:whitelist('apoapsis.core.middleclass')
        FakeRequire:fakeModule('apoapsis.core.Resource', {})
        FakeRequire:install()

        AudioBuffer = require 'apoapsis.core.AudioBuffer'
    end)

    :beforeEach(function()
        NATIVE.LoadAudioBuffer:reset()
        NATIVE.DestroyAudioBuffer:reset()
    end)

    :it('can be created and destroyed.', function()
        NATIVE.LoadAudioBuffer:canBeCalled{with={'sound.wav'}, thenReturn={'the handle'}}
        NATIVE.DestroyAudioBuffer:canBeCalled{with={'the handle'}}

        local buffer = AudioBuffer('sound.wav')
        assert(buffer.handle == 'the handle')
        NATIVE.LoadAudioBuffer:assertCallCount(1)

        buffer:destroy()
        assert(buffer.handle == nil)
        NATIVE.DestroyAudioBuffer:assertCallCount(1)
    end)


bdd.runTests()
