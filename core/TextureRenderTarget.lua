local class = require 'core/middleclass'
local RenderTarget = require 'core/RenderTarget'


local TextureRenderTarget = class('core/TextureRenderTarget', RenderTarget)

function TextureRenderTarget:initialize( texture )
    local handle = NATIVE.CreateTextureRenderTarget(texture.handle)
    RenderTarget.initialize(self, handle)
end


return TextureRenderTarget