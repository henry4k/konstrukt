local class = require 'core/middleclass'
local RenderTarget = require 'core/RenderTarget'


local DefaultRenderTarget = class('core/DefaultRenderTarget', RenderTarget)

function DefaultRenderTarget.static:get()
    return DefaultRenderTarget(NATIVE.GetDefaultRenderTarget())
end

function DefaultRenderTarget:initialize( handle )
    RenderTarget.initialize(self, handle)
end

function DefaultRenderTarget:destroy()
    -- Overridden, because default render target can't be destroyed.
    self.handle = nil
end


return DefaultRenderTarget