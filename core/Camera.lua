local class = require 'core/middleclass'


local Camera = class('core/Camera')

function Camera:initialize( modelWorld )
    self.handle = NATIVE.CreateCamera(modelWorld.handle)
    self.modelWorld = modelWorld
end

function Camera:destroy()
    NATIVE.DestroyCamera(self.handle)
    self.handle = nil
end

function Camera:getModelWorld()
    return self.modelWorld
end

function Camera:setAttachmentTarget( solid )
    NATIVE.SetCameraAttachmentTarget(self.handle, solid.handle)
    self.attachmentTarget = solid
end

function Camera:getAttachmentTarget()
    return self.attachmentTarget
end

function Camera:setViewTransformation( transformation )
    NATIVE.SetCameraViewTransformation(self.handle, transformation.handle)
end

function Camera:setFieldOfView( fov )
    NATIVE.SetCameraFieldOfView(self.handle, fov)
end


return Camera
