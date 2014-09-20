local class = require 'core/middleclass'


local RenderTarget = class('core/RenderTarget')

function RenderTarget:initialize( handle )
    self.handle = handle
    self.camera = nil
    self.shaderProgramSet = nil
end

function RenderTarget:destroy()
    NATIVE.DestroyRenderTarget(self.handle)
    self.handle = nil
end

function RenderTarget:setCamera( camera )
    NATIVE.SetRenderTargetCamera(self.handle, camera.handle)
    self.camera = camera
end

function RenderTarget:getCamera()
    return self.camera
end

function RenderTarget:setShaderProgramSet( shaderProgramSet )
    NATIVE.SetRenderTargetShaderProgramSet(self.handle, shaderProgramSet.handle)
    self.shaderProgramSet = shaderProgramSet
end

function RenderTarget:getShaderProgramSet()
    return self.shaderProgramSet
end


return RenderTarget
