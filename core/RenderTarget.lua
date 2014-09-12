local class = require 'core/middleclass'


local RenderTarget = class('core/RenderTarget')

function RenderTarget:initialize( handle )
    self.handle = handle
end

function RenderTarget:destroy()
    NATIVE.DestroyRenderTarget(self.handle)
    self.handle = nil
end

function RenderTarget:setCamera( camera )
    NATIVE.SetRenderTargetCamera(self.handle, camera.handle)
end

function RenderTarget:setShaderProgramSet( shaderProgramSet )
    NATIVE.SetRenderTargetShaderProgramSet(self.handle, shaderProgramSet.handle)
end


return RenderTarget