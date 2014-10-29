local class    = require 'middleclass'
local Resource = require 'core/Resource'
local Load2dTexture   = ENGINE.Load2dTexture
local LoadCubeTexture = ENGINE.LoadCubeTexture
local DestroyTexture  = ENGINE.DestroyTexture


local Texture = class('core/graphics/Texture')
Texture:include(Resource)

function Texture.static:_load( ... )
    local texture = Texture(...)
    return { value=texture, destructor=texture.destroy }
end

function Texture:initialize( target, fileName, ... )
    if target == '2d' then
        self.handle = Load2dTexture(fileName, ...)
    elseif target == 'cube' then
        self.handle = LoadCubeTexture(fileName, ...)
    else
        error('Unknown type: '..target)
    end
end

function Texture:destroy()
    DestroyTexture(self.handle)
    self.handle = nil
end


return Texture
