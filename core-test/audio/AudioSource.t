#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test.mock.Mock'


describe('An audio source')
    :setup(function()
        ENGINE = {
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
            ENGINE.CreateAudioSource:reset()
            ENGINE.DestroyAudioSource:reset()
            ENGINE.SetAudioSourceRelative:reset()
            ENGINE.SetAudioSourceLooping:reset()
            ENGINE.SetAudioSourcePitch:reset()
            ENGINE.SetAudioSourceGain:reset()
            ENGINE.SetAudioSourceAttachmentTarget:reset()
            ENGINE.SetAudioSourceTransformation:reset()
            ENGINE.EnqueueAudioBuffer:reset()
            ENGINE.PlayAudioSource:reset()
            ENGINE.PauseAudioSource:reset()
        end

        FakeRequire:whitelist('middleclass')
        FakeRequire:whitelist('core/audio/AudioSource')
        FakeRequire:install()

        AudioSource = require 'core/audio/AudioSource'
    end)

    :beforeEach(function()
        ResetMocks()
    end)

    :it('can be created and destroyed.', function()
        ENGINE.CreateAudioSource:canBeCalled{thenReturn={'source handle'}}
        ENGINE.DestroyAudioSource:canBeCalled{with={'source handle'}}

        local source = AudioSource()
        assert(source.handle == 'source handle')
        ENGINE.CreateAudioSource:assertCallCount(1)

        source:destroy()
        assert(source.handle == nil)
        ENGINE.DestroyAudioSource:assertCallCount(1)
    end)

    :beforeEach(function()
        ResetMocks()

        ENGINE.CreateAudioSource:canBeCalled{thenReturn={'source handle'} }
        ENGINE.DestroyAudioSource:canBeCalled{with={'source handle'}}
        Source = AudioSource()
    end)

    :afterEach(function()
        Source:destroy()
        Source = nil
    end)

    :it('can be relative to listener.', function()
        ENGINE.SetAudioSourceRelative:canBeCalled{with={'source handle', true}}
        Source:setRelative(true)
        ENGINE.SetAudioSourceRelative:assertCallCount(1)
    end)

    :it('can loop.', function()
        ENGINE.SetAudioSourceLooping:canBeCalled{with={'source handle', true}}
        Source:setLooping(true)
        ENGINE.SetAudioSourceLooping:assertCallCount(1)
    end)

    :it('has a pitch.', function()
        ENGINE.SetAudioSourcePitch:canBeCalled{with={'source handle', 1.2}}
        Source:setPitch(1.2)
        ENGINE.SetAudioSourcePitch:assertCallCount(1)
    end)

    :it('has a gain.', function()
        ENGINE.SetAudioSourceGain:canBeCalled{with={'source handle', 1.2}}
        Source:setGain(1.2)
        ENGINE.SetAudioSourceGain:assertCallCount(1)
    end)

    :it('has an attachment target.', function()
        ENGINE.SetAudioSourceAttachmentTarget:canBeCalled{with={'source handle', 'solid handle'}}
        local solid = { handle = 'solid handle' }
        Source:setAttachmentTarget(solid)
        ENGINE.SetAudioSourceAttachmentTarget:assertCallCount(1)
    end)

    :it('has a transformation.', function()
        ENGINE.SetAudioSourceTransformation:canBeCalled{with={'source handle', 'matrix handle'}}
        local matrix = { handle = 'matrix handle' }
        Source:setTransformation(matrix)
        ENGINE.SetAudioSourceTransformation:assertCallCount(1)
    end)

    :it('can enqueue buffers.', function()
        ENGINE.EnqueueAudioBuffer:canBeCalled{with={'source handle', 'buffer handle'}}
        local buffer = { handle = 'buffer handle' }
        Source:enqueue(buffer)
        ENGINE.EnqueueAudioBuffer:assertCallCount(1)
    end)

    :it('can be played.', function()
        ENGINE.PlayAudioSource:canBeCalled{with={'source handle'}}
        Source:play()
        ENGINE.PlayAudioSource:assertCallCount(1)
    end)

    :it('can be paused.', function()
        ENGINE.PauseAudioSource:canBeCalled{with={'source handle'}}
        Source:pause()
        ENGINE.PauseAudioSource:assertCallCount(1)
    end)


bdd.runTests()
