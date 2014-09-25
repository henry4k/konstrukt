local class        = require 'middleclass'
local RenderTarget = require 'core/RenderTarget'
local CreateTextureRenderTarget = ENGINE.CreateTextureRenderTarget


local TextureRenderTarget = class('core/TextureRenderTarget', RenderTarget)

function TextureRenderTarget:initialize( texture )
    local handle = CreateTextureRenderTarget(texture.handle)
    RenderTarget.initialize(self, handle)
end


return TextureRenderTarget
