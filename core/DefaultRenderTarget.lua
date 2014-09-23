local class = require 'core/middleclass'
local RenderTarget = require 'core/RenderTarget'


local DefaultRenderTarget = class('core/DefaultRenderTarget', RenderTarget)

function DefaultRenderTarget.static:get()
    if not self.instance then
        local handle = NATIVE.GetDefaultRenderTarget()
        self.instance = DefaultRenderTarget(handle)
    end
    return self.instance
end

function DefaultRenderTarget:initialize( handle )
    RenderTarget.initialize(self, handle)
end

function DefaultRenderTarget:destroy()
    -- Overridden, because default render target can't be destroyed.
    self.handle = nil
end


return DefaultRenderTarget
