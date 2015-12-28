--- @classmod core.graphics.TextureRenderTarget
--- Renders to a texture.
--
-- Extends @{core.graphics.RenderTarget}.


local engine = require 'engine'
local class  = require 'middleclass'
local Object = class.Object
local RenderTarget = require 'core/graphics/RenderTarget'
local Texture      = require 'core/graphics/Texture'


local TextureRenderTarget = class('core/graphics/TextureRenderTarget', RenderTarget)

---
-- @param[type=core.graphics.Texture] texture
--
function TextureRenderTarget:initialize( texture )
    assert(Object.isInstanceOf(texture, Texture),
           'Must be initialized with a texture.')
    local handle = engine.CreateTextureRenderTarget(texture.handle)
    RenderTarget.initialize(self, handle)
end


return TextureRenderTarget
