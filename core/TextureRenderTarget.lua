local class        = require 'middleclass'
local RenderTarget = require 'apoapsis.core.RenderTarget'
local engine       = require 'apoapsis.engine'
local CreateTextureRenderTarget = engine.CreateTextureRenderTarget


local TextureRenderTarget = class('apoapsis/core/TextureRenderTarget', RenderTarget)

function TextureRenderTarget:initialize( texture )
    local handle = CreateTextureRenderTarget(texture.handle)
    RenderTarget.initialize(self, handle)
end


return TextureRenderTarget
