local class  = require 'middleclass'
local DestroyRenderTarget             = ENGINE.DestroyRenderTarget
local SetRenderTargetCamera           = ENGINE.SetRenderTargetCamera
local SetRenderTargetShaderProgramSet = ENGINE.SetRenderTargetShaderProgramSet


local RenderTarget = class('core/RenderTarget')

function RenderTarget:initialize( handle )
    self.handle = handle
    self.cameras = {}
    self.shaderProgramSet = nil
end

function RenderTarget:destroy()
    DestroyRenderTarget(self.handle)
    self.handle = nil
end

--- A render target can use multiple cameras.
-- Geometry of cameras with a lower layer can't occlude geometry from higher
-- layers.  This can be used to separate HUD and background from the regular
-- scene.
function RenderTarget:setCamera( camera, layer )
    SetRenderTargetCamera(self.handle, camera.handle, layer-1)
    self.cameras[layer] = camera
end

function RenderTarget:getCamera( layer )
    return self.cameras[layer]
end

function RenderTarget:setShaderProgramSet( shaderProgramSet )
    SetRenderTargetShaderProgramSet(self.handle, shaderProgramSet.handle)
    self.shaderProgramSet = shaderProgramSet
end

function RenderTarget:getShaderProgramSet()
    return self.shaderProgramSet
end


return RenderTarget
