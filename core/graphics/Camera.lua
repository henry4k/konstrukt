---
-- @module core.graphics.Camera


local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Mat4   = require 'core/Matrix4'
local ModelWorld          = require 'core/graphics/ModelWorld'
local HasAttachmentTarget = require 'core/physics/HasAttachmentTarget'
local CreateCamera                = ENGINE.CreateCamera
local DestroyCamera               = ENGINE.DestroyCamera
local SetCameraAttachmentTarget   = ENGINE.SetCameraAttachmentTarget
local SetCameraViewTransformation = ENGINE.SetCameraViewTransformation
local SetCameraNearAndFarPlanes   = ENGINE.SetCameraNearAndFarPlanes
local SetCameraProjectionType     = ENGINE.SetCameraProjectionType


local Camera = class('core/graphics/Camera')
Camera:include(HasAttachmentTarget)

---
-- @warning DON'T CALL THIS DIRECTLY!
-- Use `PerspectiveCamera` or `OrthographicCamera` instead.
function Camera:initialize( modelWorld, projectionType )
    assert(Object.isInstanceOf(modelWorld, ModelWorld), 'Must be initialized with a model world.')
    self.handle = CreateCamera(modelWorld.handle)
    self.modelWorld = modelWorld
    SetCameraProjectionType(self.handle, projectionType)
end

function Camera:destroy()
    DestroyCamera(self.handle)
    self.handle = nil
end

function Camera:getModelWorld()
    return self.modelWorld
end

function Camera:setViewTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    SetCameraViewTransformation(self.handle, matrix.handle)
end

function Camera:setNearAndFarPlanes( zNear, zFar )
    assert(zNear > 0 and zFar > 0, 'z near and z far must be greater than zero.')
    assert(zFar > zNear, 'z far must be larger than z near.')
    SetCameraNearAndFarPlanes(self.handle, zNear, zFar)
end

function Camera:_setAttachmentTarget( solidHandle, flags )
    SetCameraAttachmentTarget(self.handle, solidHandle, flags)
end


return Camera
