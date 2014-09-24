#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'apoapsis.core.test.common'

local Mock = require 'test.mock.Mock'


describe('An audio source')
    :setup(function()
        NATIVE = {
            CreateAudioSource = Mock(),
            DestroyAudioSource = Mock(),
            SetAudioSourceRelative = Mock(),
            SetAudioSourceLooping = Mock(),
            SetAudioSourcePitch = Mock(),
            SetAudioSourceGain = Mock(),
            SetAudioSourceAttachmentTarget = Mock(),
            SetAudioSourceTransformation = Mock(),
            EnqueueAudioBuffer = Mock(),
            PlayAudioSource = Mock(),
            PauseAudioSource = Mock()
        }

        ResetMocks = function()
            NATIVE.CreateAudioSource:reset()
            NATIVE.DestroyAudioSource:reset()
            NATIVE.SetAudioSourceRelative:reset()
            NATIVE.SetAudioSourceLooping:reset()
            NATIVE.SetAudioSourcePitch:reset()
            NATIVE.SetAudioSourceGain:reset()
            NATIVE.SetAudioSourceAttachmentTarget:reset()
            NATIVE.SetAudioSourceTransformation:reset()
            NATIVE.EnqueueAudioBuffer:reset()
            NATIVE.PlayAudioSource:reset()
            NATIVE.PauseAudioSource:reset()
        end

        FakeRequire:whitelist('apoapsis.core.AudioSource')
        FakeRequire:whitelist('apoapsis.core.middleclass')
        FakeRequire:install()

        AudioSource = require 'apoapsis.core.AudioSource'
    end)

    :beforeEach(function()
        ResetMocks()
    end)

    :it('can be created and destroyed.', function()
        NATIVE.CreateAudioSource:canBeCalled{thenReturn={'source handle'}}
        NATIVE.DestroyAudioSource:canBeCalled{with={'source handle'}}

        local source = AudioSource()
        assert(source.handle == 'source handle')
        NATIVE.CreateAudioSource:assertCallCount(1)

        source:destroy()
        assert(source.handle == nil)
        NATIVE.DestroyAudioSource:assertCallCount(1)
    end)

    :beforeEach(function()
        ResetMocks()

        NATIVE.CreateAudioSource:canBeCalled{thenReturn={'source handle'} }
        NATIVE.DestroyAudioSource:canBeCalled{with={'source handle'}}
        Source = AudioSource()
    end)

    :afterEach(function()
        Source:destroy()
        Source = nil
    end)

    :it('can be relative to listener.', function()
        NATIVE.SetAudioSourceRelative:canBeCalled{with={'source handle', true}}
        Source:setRelative(true)
        NATIVE.SetAudioSourceRelative:assertCallCount(1)
    end)

    :it('can loop.', function()
        NATIVE.SetAudioSourceLooping:canBeCalled{with={'source handle', true}}
        Source:setLooping(true)
        NATIVE.SetAudioSourceLooping:assertCallCount(1)
    end)

    :it('has a pitch.', function()
        NATIVE.SetAudioSourcePitch:canBeCalled{with={'source handle', 1.2}}
        Source:setPitch(1.2)
        NATIVE.SetAudioSourcePitch:assertCallCount(1)
    end)

    :it('has a gain.', function()
        NATIVE.SetAudioSourceGain:canBeCalled{with={'source handle', 1.2}}
        Source:setGain(1.2)
        NATIVE.SetAudioSourceGain:assertCallCount(1)
    end)

    :it('has an attachment target.', function()
        NATIVE.SetAudioSourceAttachmentTarget:canBeCalled{with={'source handle', 'solid handle'}}
        local solid = { handle = 'solid handle' }
        Source:setAttachmentTarget(solid)
        NATIVE.SetAudioSourceAttachmentTarget:assertCallCount(1)
    end)

    :it('has a transformation.', function()
        NATIVE.SetAudioSourceTransformation:canBeCalled{with={'source handle', 'matrix handle'}}
        local matrix = { handle = 'matrix handle' }
        Source:setTransformation(matrix)
        NATIVE.SetAudioSourceTransformation:assertCallCount(1)
    end)

    :it('can enqueue buffers.', function()
        NATIVE.EnqueueAudioBuffer:canBeCalled{with={'source handle', 'buffer handle'}}
        local buffer = { handle = 'buffer handle' }
        Source:enqueue(buffer)
        NATIVE.EnqueueAudioBuffer:assertCallCount(1)
    end)

    :it('can be played.', function()
        NATIVE.PlayAudioSource:canBeCalled{with={'source handle'}}
        Source:play()
        NATIVE.PlayAudioSource:assertCallCount(1)
    end)

    :it('can be paused.', function()
        NATIVE.PauseAudioSource:canBeCalled{with={'source handle'}}
        Source:pause()
        NATIVE.PauseAudioSource:assertCallCount(1)
    end)


bdd.runTests()
