local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Solid  = require 'core/Solid'
local ModelWorld = require 'core/ModelWorld'
local CreateCamera                = ENGINE.CreateCamera
local DestroyCamera               = ENGINE.DestroyCamera
local SetCameraAttachmentTarget   = ENGINE.SetCameraAttachmentTarget
local SetCameraViewTransformation = ENGINE.SetCameraViewTransformation
local SetCameraFieldOfView        = ENGINE.SetCameraFieldOfView
local SetCameraNearAndFarPlanes   = ENGINE.SetCameraNearAndFarPlanes


local Camera = class('core/Camera')

function Camera:initialize( modelWorld )
    assert(Object.isInstanceOf(modelWorld, ModelWorld), 'Must be initialized with a model world.')
    self.handle = CreateCamera(modelWorld.handle)
    self.modelWorld = modelWorld
end

function Camera:destroy()
    DestroyCamera(self.handle)
    self.handle = nil
end

function Camera:getModelWorld()
    return self.modelWorld
end

function Camera:setAttachmentTarget( solid, flags )
    assert(Object.isInstanceOf(solid, Solid), 'Attachment target must be a solid.')
    flags = flags or 'rt'
    SetCameraAttachmentTarget(self.handle, solid.handle, flags)
    self.attachmentTarget = solid
end

function Camera:getAttachmentTarget()
    return self.attachmentTarget
end

function Camera:setViewTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    SetCameraViewTransformation(self.handle, matrix.handle)
end

function Camera:setFieldOfView( fov )
    SetCameraFieldOfView(self.handle, fov)
end

function Camera:setNearAndFarPlanes( zNear, zFar )
    SetCameraNearAndFarPlanes(self.handle, zNear, zFar)
end


return Camera
