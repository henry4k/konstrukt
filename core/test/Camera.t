#!/usr/bin/env lua
-- vim: set filetype=lua:
require 'core/test/common'

local Mock = require 'test/mock/Mock'


describe('A camera')
    :setup(function()
        NATIVE = {
            CreateCamera = Mock(),
            DestroyCamera = Mock(),
            SetCameraAttachmentTarget = Mock(),
            SetCameraViewTransformation = Mock(),
            SetCameraFieldOfView = Mock()
        }

        ResetMocks = function()
            NATIVE.CreateCamera:reset()
            NATIVE.DestroyCamera:reset()
            NATIVE.SetCameraAttachmentTarget:reset()
            NATIVE.SetCameraViewTransformation:reset()
            NATIVE.SetCameraFieldOfView:reset()
        end

        FakeRequire:whitelist('core/Camera')
        FakeRequire:whitelist('core/middleclass')
        FakeRequire:install()

        Camera = require 'core/Camera'
    end)

    :beforeEach(ResetMocks)

    :it('can be created and destroyed.', function()
        NATIVE.CreateCamera:whenCalledWith{'model world handle', returns={'camera handle'}}
        NATIVE.DestroyCamera:whenCalledWith{'camera handle'}

        local modelWorld = { handle = 'model world handle' }
        local camera = Camera(modelWorld)
        assert(camera.handle == 'camera handle')
        NATIVE.CreateCamera:assertCallCount(1)

        camera:destroy()
        assert(camera.handle == nil)
        NATIVE.DestroyCamera:assertCallCount(1)
    end)

    :beforeEach(function()
        ResetMocks()

        NATIVE.CreateCamera:whenCalledWith{'model world handle', returns={'camera handle'}}
        local modelWorld = { handle = 'model world handle' }
        camera = Camera(modelWorld)
    end)

    :afterEach(function()
        NATIVE.DestroyCamera:whenCalledWith{'camera handle'}
        camera:destroy()
    end)

    :it('has an attachment target.', function()
        NATIVE.SetCameraAttachmentTarget:whenCalledWith{'camera handle', 'solid handle'}
        local solid = { handle = 'solid handle' }
        camera:setAttachmentTarget(solid)
        NATIVE.SetCameraAttachmentTarget:assertCallCount(1)
    end)

    :it('has a view transformation.', function()
        NATIVE.SetCameraViewTransformation:whenCalledWith{'camera handle', 'matrix handle'}
        local matrix = { handle = 'matrix handle' }
        camera:setViewTransformation(matrix)
        NATIVE.SetCameraViewTransformation:assertCallCount(1)
    end)

    :it('has an field of view.', function()
        NATIVE.SetCameraFieldOfView:whenCalledWith{'camera handle', 90}
        camera:setFieldOfView(90)
        NATIVE.SetCameraFieldOfView:assertCallCount(1)
    end)


bdd.runTests()
