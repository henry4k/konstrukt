#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core-test/common'

local Mock = require 'test.mock.Mock'
local class = require 'middleclass'

describe('The audio listener')
    :setup(function()
        ENGINE = {
            SetAudioListenerAttachmentTarget = Mock(),
            SetAudioListenerTransformation = Mock()
        }

        Solid = class('Fake Solid')
        Matrix4 = class('Fake Matrix4')

        FakeRequire:whitelist('middleclass')
        FakeRequire:whitelist('core/audio/AudioListener')
        FakeRequire:fakeModule('core/physics/Solid', Solid)
        FakeRequire:fakeModule('core/Matrix4', Matrix4)
        FakeRequire:install()

        AudioListener = require 'core/audio/AudioListener'
    end)

    :beforeEach(function()
        ENGINE.SetAudioListenerAttachmentTarget:reset()
        ENGINE.SetAudioListenerTransformation:reset()
    end)

    :it('has an attachment target.', function()
        ENGINE.SetAudioListenerAttachmentTarget:canBeCalled{with={'solid handle', 'rt'}}
        local solid = Solid()
        solid.handle = 'solid handle'
        AudioListener.setAttachmentTarget(solid)
        ENGINE.SetAudioListenerAttachmentTarget:assertCallCount(1)
    end)

    :it('has a transformation.', function()
        ENGINE.SetAudioListenerTransformation:canBeCalled{with={'matrix handle'}}
        local matrix = Matrix4()
        matrix.handle = 'matrix handle'
        AudioListener.setTransformation(matrix)
        ENGINE.SetAudioListenerTransformation:assertCallCount(1)
    end)


bdd.runTests()
