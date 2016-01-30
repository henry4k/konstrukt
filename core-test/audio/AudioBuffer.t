#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core-test/common'

local Mock = require 'test.mock.Mock'


describe('An audio buffer')
    :setup(function()
        ENGINE = {
            LoadAudioBuffer = Mock(),
            DestroyAudioBuffer = Mock()
        }

        FakeRequire:whitelist('middleclass')
        FakeRequire:whitelist('core/audio/AudioBuffer')
        FakeRequire:fakeModule('core/Resource', {})
        FakeRequire:install()

        AudioBuffer = require 'core/audio/AudioBuffer'
    end)

    :beforeEach(function()
        ENGINE.LoadAudioBuffer:reset()
        ENGINE.DestroyAudioBuffer:reset()
    end)

    :it('can be created and destroyed.', function()
        ENGINE.LoadAudioBuffer:canBeCalled{with={'sound.wav'}, thenReturn={'the handle'}}
        ENGINE.DestroyAudioBuffer:canBeCalled{with={'the handle'}}

        local buffer = AudioBuffer('sound.wav')
        assert(buffer.handle == 'the handle')
        ENGINE.LoadAudioBuffer:assertCallCount(1)

        buffer:destroy()
        assert(buffer.handle == nil)
        ENGINE.DestroyAudioBuffer:assertCallCount(1)
    end)


bdd.runTests()
