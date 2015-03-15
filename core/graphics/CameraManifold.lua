--- @classmod core.graphics.CameraManifold
--- Controls multiple cameras simultaneously.
--
-- *This is still under construction and will probably change in the future.*


local assert       = assert
local class        = require 'middleclass'
local Object       = class.Object
local RenderTarget = require 'core/graphics/RenderTarget'
local PerspectiveCamera = require 'core/graphics/PerspectiveCamera'


local CameraManifold = class('core/graphics/CameraManifold', RenderTarget)

---
-- @param renderTarget
--
function CameraManifold:initialize( renderTarget )
    assert(Object.isInstanceOf(renderTarget, RenderTarget), 'Must be initialized with a render target.')
    self.renderTarget = renderTarget
    self.foregroundCamera = renderTarget:getCameraByName('foreground')
    self.worldCamera      = renderTarget:getCameraByName('world')
    self.backgroundCamera = renderTarget:getCameraByName('background')
    assert(Object.isInstanceOf(self.foregroundCamera, PerspectiveCamera) and
           Object.isInstanceOf(self.worldCamera,      PerspectiveCamera) and
           Object.isInstanceOf(self.backgroundCamera, PerspectiveCamera),
           'The render target must have perspective foreground, world and background cameras set.')
    self:setAttachmentTarget(nil)
end

function CameraManifold:destroy()
end

function CameraManifold:getModelWorld()
    return self.worldCamera:getModelWorld()
end

--- Change the cameras attachment target.
function CameraManifold:setAttachmentTarget( solid )
    self.attachmentTarget = solid
    self.worldCamera:setAttachmentTarget(solid, 'rt')
    self.backgroundCamera:setAttachmentTarget(solid, 'r')
end

function CameraManifold:getAttachmentTarget()
    return self.attachmentTarget
end

function CameraManifold:setViewTransformation( matrix )
    self.worldCamera:setViewTransformation(matrix)
    self.backgroundCamera:setViewTransformation(matrix:toRotationMatrix())
end

function CameraManifold:setFieldOfView( fov )
    self.foregroundCamera:setFieldOfView(fov)
    self.worldCamera:setFieldOfView(fov)
    self.backgroundCamera:setFieldOfView(fov)
end


return CameraManifold
