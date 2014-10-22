local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local RenderTarget = require 'core/RenderTarget'
local Texture      = require 'core/Texture'
local CreateTextureRenderTarget = ENGINE.CreateTextureRenderTarget


local TextureRenderTarget = class('core/TextureRenderTarget', RenderTarget)

function TextureRenderTarget:initialize( texture )
    assert(Object.isInstanceOf(texture, Texture),
           'Must be initialized with a texture.')
    local handle = CreateTextureRenderTarget(texture.handle)
    RenderTarget.initialize(self, handle)
end


return TextureRenderTarget
