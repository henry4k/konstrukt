local class  = require 'middleclass'
local CreateCamera                = ENGINE.CreateCamera
local DestroyCamera               = ENGINE.DestroyCamera
local SetCameraAttachmentTarget   = ENGINE.SetCameraAttachmentTarget
local SetCameraViewTransformation = ENGINE.SetCameraViewTransformation
local SetCameraFieldOfView        = ENGINE.SetCameraFieldOfView


local Camera = class('core/Camera')

function Camera:initialize( modelWorld )
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

function Camera:setAttachmentTarget( solid )
    SetCameraAttachmentTarget(self.handle, solid.handle)
    self.attachmentTarget = solid
end

function Camera:getAttachmentTarget()
    return self.attachmentTarget
end

function Camera:setViewTransformation( transformation )
    SetCameraViewTransformation(self.handle, transformation.handle)
end

function Camera:setFieldOfView( fov )
    SetCameraFieldOfView(self.handle, fov)
end


return Camera
