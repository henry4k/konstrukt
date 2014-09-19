#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test/mock/Mock'


describe('The audio listener')
    :setup(function()
        NATIVE = {
            SetAudioListenerAttachmentTarget = Mock(),
            SetAudioListenerTransformation = Mock()
        }

        FakeRequire:whitelist('core/AudioListener')
        FakeRequire:install()

        AudioListener = require 'core/AudioListener'
    end)

    :beforeEach(function()
        NATIVE.SetAudioListenerAttachmentTarget:reset()
        NATIVE.SetAudioListenerTransformation:reset()
    end)

    :it('has an attachment target.', function()
        NATIVE.SetAudioListenerAttachmentTarget:whenCalledWith{'solid handle'}
        local solid = { handle = 'solid handle' }
        AudioListener.setAttachmentTarget(solid)
        NATIVE.SetAudioListenerAttachmentTarget:assertCallCount(1)
    end)

    :it('has a transformation.', function()
        NATIVE.SetAudioListenerTransformation:whenCalledWith{'matrix handle'}
        local matrix = { handle = 'matrix handle' }
        AudioListener.setTransformation(matrix)
        NATIVE.SetAudioListenerTransformation:assertCallCount(1)
    end)


bdd.runTests()
