--- @classmod core.graphics.DefaultRenderTarget
--- Render target for the default frame buffer. (Aka window system frame buffer.)
--
-- Extends @{core.graphics.RenderTarget}.


local engine       = require 'engine'
local class        = require 'middleclass'
local Scheduler    = require 'core/Scheduler'
local RenderTarget = require 'core/graphics/RenderTarget'


local DefaultRenderTarget = class('core/graphics/DefaultRenderTarget', RenderTarget)

function DefaultRenderTarget:initialize()
    assert(not DefaultRenderTarget.instance, 'Use DefaultRenderTarget:get() instead.')
    RenderTarget.initialize(self, Scheduler.awaitCall(engine.GetDefaultRenderTarget))
end

function DefaultRenderTarget:destroy()
    -- Overridden, because default render target can't be destroyed.
    self.handle = nil
end


return DefaultRenderTarget()
