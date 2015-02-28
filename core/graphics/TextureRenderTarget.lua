--- @classmod core.graphics.TextureRenderTarget
--- Renders to a texture.
--
-- Extends @{RenderTarget}.


local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local RenderTarget = require 'core/graphics/RenderTarget'
local Texture      = require 'core/graphics/Texture'
local CreateTextureRenderTarget = ENGINE.CreateTextureRenderTarget


local TextureRenderTarget = class('core/graphics/TextureRenderTarget', RenderTarget)

---
-- @param[type=Texture] texture
--
function TextureRenderTarget:initialize( texture )
    assert(Object.isInstanceOf(texture, Texture),
           'Must be initialized with a texture.')
    local handle = CreateTextureRenderTarget(texture.handle)
    RenderTarget.initialize(self, handle)
end


return TextureRenderTarget
