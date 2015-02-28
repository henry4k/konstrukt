--- @classmod core.graphics.DefaultRenderTarget
--- Render target for the default frame buffer. (Aka window system frame buffer.)
--
-- Extends @{RenderTarget}.


local class        = require 'middleclass'
local RenderTarget = require 'core/graphics/RenderTarget'
local GetDefaultRenderTarget = ENGINE.GetDefaultRenderTarget


local DefaultRenderTarget = class('core/graphics/DefaultRenderTarget', RenderTarget)

--- Retrieve the singleton.
function DefaultRenderTarget.static:get()
    if not self.instance then
        self.instance = DefaultRenderTarget()
    end
    return self.instance
end

function DefaultRenderTarget:initialize()
    assert(not DefaultRenderTarget.instance, 'Use DefaultRenderTarget:get() instead.')
    RenderTarget.initialize(self, GetDefaultRenderTarget())
end

function DefaultRenderTarget:destroy()
    -- Overridden, because default render target can't be destroyed.
    self.handle = nil
end


return DefaultRenderTarget
