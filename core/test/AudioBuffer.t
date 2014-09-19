#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test/mock/Mock'


describe('An audio buffer')
    :setup(function()
        NATIVE = {
            LoadAudioBuffer = Mock(),
            DestroyAudioBuffer = Mock()
        }

        FakeRequire:whitelist('core/AudioBuffer')
        FakeRequire:whitelist('core/middleclass')
        FakeRequire:fakeModule('core/Resource', {})
        FakeRequire:install()

        AudioBuffer = require 'core/AudioBuffer'
    end)

    :beforeEach(function()
        NATIVE.LoadAudioBuffer:reset()
        NATIVE.DestroyAudioBuffer:reset()
    end)

    :it('can be created and destroyed.', function()
        NATIVE.LoadAudioBuffer:whenCalledWith{'sound.wav', returns={'the handle'}}
        NATIVE.DestroyAudioBuffer:whenCalledWith{'the handle'}

        local buffer = AudioBuffer('sound.wav')
        assert(buffer.handle == 'the handle')
        NATIVE.LoadAudioBuffer:assertCallCount(1)

        buffer:destroy()
        assert(buffer.handle == nil)
        NATIVE.DestroyAudioBuffer:assertCallCount(1)
    end)


bdd.runTests()
