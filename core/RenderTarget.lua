local class  = require 'middleclass'
local engine = require 'apoapsis.engine'
local DestroyRenderTarget             = engine.DestroyRenderTarget
local SetRenderTargetCamera           = engine.SetRenderTargetCamera
local SetRenderTargetShaderProgramSet = engine.SetRenderTargetShaderProgramSet


local RenderTarget = class('apoapsis/core/RenderTarget')

function RenderTarget:initialize( handle )
    self.handle = handle
    self.camera = nil
    self.shaderProgramSet = nil
end

function RenderTarget:destroy()
    DestroyRenderTarget(self.handle)
    self.handle = nil
end

function RenderTarget:setCamera( camera )
    SetRenderTargetCamera(self.handle, camera.handle)
    self.camera = camera
end

function RenderTarget:getCamera()
    return self.camera
end

function RenderTarget:setShaderProgramSet( shaderProgramSet )
    SetRenderTargetShaderProgramSet(self.handle, shaderProgramSet.handle)
    self.shaderProgramSet = shaderProgramSet
end

function RenderTarget:getShaderProgramSet()
    return self.shaderProgramSet
end


return RenderTarget
