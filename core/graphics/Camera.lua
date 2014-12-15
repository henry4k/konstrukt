--- Cameras define the transformation which is used to render the associated @{ModelWorld} to the @{RenderTarget}.
--
-- Includes @{HasAttachmentTarget}.
--
-- @classmod core.graphics.Camera


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

--- Abstract class.  Use @{PerspectiveCamera} or @{OrthographicCamera} instead.
function Camera:initialize( modelWorld, projectionType )
    assert(self.class ~= Camera, 'Camera is an abstract class and not meant to be instanciated directly.')
    assert(Object.isInstanceOf(modelWorld, ModelWorld), 'Must be initialized with a model world.')
    self.handle = CreateCamera(modelWorld.handle)
    self.modelWorld = modelWorld
    SetCameraProjectionType(self.handle, projectionType)
end

function Camera:destroy()
    DestroyCamera(self.handle)
    self.handle = nil
end

--- Retrieve @{ModelWorld} used by the camera.
function Camera:getModelWorld()
    return self.modelWorld
end

--- Change the cameras view transformation matrix.
function Camera:setViewTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    SetCameraViewTransformation(self.handle, matrix.handle)
end

--- Set the cameras view range.
--
-- Due to the design of the rendering system, cameras can't display infinitly
-- near or far objects.
--
function Camera:setNearAndFarPlanes( zNear, zFar )
    assert(zNear > 0 and zFar > 0, 'z near and z far must be greater than zero.')
    assert(zFar > zNear, 'z far must be larger than z near.')
    SetCameraNearAndFarPlanes(self.handle, zNear, zFar)
end

function Camera:_setAttachmentTarget( solidHandle, flags )
    SetCameraAttachmentTarget(self.handle, solidHandle, flags)
end


return Camera
