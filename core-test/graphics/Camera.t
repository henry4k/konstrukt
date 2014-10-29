#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test.mock.Mock'


describe('A camera')
    :setup(function()
        ENGINE = {
            CreateCamera = Mock(),
            DestroyCamera = Mock(),
            SetCameraAttachmentTarget = Mock(),
            SetCameraViewTransformation = Mock(),
            SetCameraFieldOfView = Mock()
        }

        ResetMocks = function()
            ENGINE.CreateCamera:reset()
            ENGINE.DestroyCamera:reset()
            ENGINE.SetCameraAttachmentTarget:reset()
            ENGINE.SetCameraViewTransformation:reset()
            ENGINE.SetCameraFieldOfView:reset()
        end

        FakeRequire:whitelist('middleclass')
        FakeRequire:whitelist('core/graphics/Camera')
        FakeRequire:install()

        Camera = require 'core/graphics/Camera'
    end)

    :beforeEach(function()
        ResetMocks()
    end)

    :it('can be created and destroyed.', function()
        ENGINE.CreateCamera:canBeCalled{with={'model world handle'}, thenReturn={'camera handle'}}
        ENGINE.DestroyCamera:canBeCalled{with={'camera handle'}}

        local modelWorld = { handle = 'model world handle' }
        local camera = Camera(modelWorld)
        assert(camera.handle == 'camera handle')
        ENGINE.CreateCamera:assertCallCount(1)

        camera:destroy()
        assert(camera.handle == nil)
        ENGINE.DestroyCamera:assertCallCount(1)
    end)

    :beforeEach(function()
        ResetMocks()

        ENGINE.CreateCamera:canBeCalled{with={'model world handle'}, thenReturn={'camera handle'}}
        local modelWorld = { handle = 'model world handle' }
        camera = Camera(modelWorld)
    end)

    :it('has an attachment target.', function()
        ENGINE.SetCameraAttachmentTarget:canBeCalled{with={'camera handle', 'solid handle'}}
        local solid = { handle = 'solid handle' }
        camera:setAttachmentTarget(solid)
        ENGINE.SetCameraAttachmentTarget:assertCallCount(1)
    end)

    :it('has a view transformation.', function()
        ENGINE.SetCameraViewTransformation:canBeCalled{with={'camera handle', 'matrix handle'}}
        local matrix = { handle = 'matrix handle' }
        camera:setViewTransformation(matrix)
        ENGINE.SetCameraViewTransformation:assertCallCount(1)
    end)

    :it('has an field of view.', function()
        ENGINE.SetCameraFieldOfView:canBeCalled{with={'camera handle', 90}}
        camera:setFieldOfView(90)
        ENGINE.SetCameraFieldOfView:assertCallCount(1)
    end)


bdd.runTests()
