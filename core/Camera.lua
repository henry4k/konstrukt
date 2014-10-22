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
    assert(fov > 0, 'FoV must be greater than zero.')
    SetCameraFieldOfView(self.handle, fov)
end

function Camera:setNearAndFarPlanes( zNear, zFar )
    assert(zNear > 0 and zFar > 0, 'z near and z far must be greater than zero.')
    assert(zFar > zNear, 'z far must be larger than z near.')
    SetCameraNearAndFarPlanes(self.handle, zNear, zFar)
end


return Camera
