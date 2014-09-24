#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'apoapsis.core.test.common'

local Mock = require 'test.mock.Mock'


describe('The audio listener')
    :setup(function()
        NATIVE = {
            SetAudioListenerAttachmentTarget = Mock(),
            SetAudioListenerTransformation = Mock()
        }

        FakeRequire:whitelist('apoapsis.core.AudioListener')
        FakeRequire:install()

        AudioListener = require 'apoapsis.core.AudioListener'
    end)

    :beforeEach(function()
        NATIVE.SetAudioListenerAttachmentTarget:reset()
        NATIVE.SetAudioListenerTransformation:reset()
    end)

    :it('has an attachment target.', function()
        NATIVE.SetAudioListenerAttachmentTarget:canBeCalled{with={'solid handle'}}
        local solid = { handle = 'solid handle' }
        AudioListener.setAttachmentTarget(solid)
        NATIVE.SetAudioListenerAttachmentTarget:assertCallCount(1)
    end)

    :it('has a transformation.', function()
        NATIVE.SetAudioListenerTransformation:canBeCalled{with={'matrix handle'}}
        local matrix = { handle = 'matrix handle' }
        AudioListener.setTransformation(matrix)
        NATIVE.SetAudioListenerTransformation:assertCallCount(1)
    end)


bdd.runTests()
