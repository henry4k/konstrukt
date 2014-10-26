#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test.mock.Mock'


describe('The audio listener')
    :setup(function()
        ENGINE = {
            SetAudioListenerAttachmentTarget = Mock(),
            SetAudioListenerTransformation = Mock()
        }

        FakeRequire:whitelist('core/audio/AudioListener')
        FakeRequire:install()

        AudioListener = require 'core/audio/AudioListener'
    end)

    :beforeEach(function()
        ENGINE.SetAudioListenerAttachmentTarget:reset()
        ENGINE.SetAudioListenerTransformation:reset()
    end)

    :it('has an attachment target.', function()
        ENGINE.SetAudioListenerAttachmentTarget:canBeCalled{with={'solid handle'}}
        local solid = { handle = 'solid handle' }
        AudioListener.setAttachmentTarget(solid)
        ENGINE.SetAudioListenerAttachmentTarget:assertCallCount(1)
    end)

    :it('has a transformation.', function()
        ENGINE.SetAudioListenerTransformation:canBeCalled{with={'matrix handle'}}
        local matrix = { handle = 'matrix handle' }
        AudioListener.setTransformation(matrix)
        ENGINE.SetAudioListenerTransformation:assertCallCount(1)
    end)


bdd.runTests()
