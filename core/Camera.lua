local class  = require 'middleclass'
local engine = require 'apoapsis.engine'
local CreateCamera                = engine.CreateCamera
local DestroyCamera               = engine.DestroyCamera
local SetCameraAttachmentTarget   = engine.SetCameraAttachmentTarget
local SetCameraViewTransformation = engine.SetCameraViewTransformation
local SetCameraFieldOfView        = engine.SetCameraFieldOfView


local Camera = class('apoapsis/core/Camera')

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
