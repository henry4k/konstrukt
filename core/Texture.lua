local class = require 'core/middleclass'
local Resource = require 'core/Resource'


local Texture = class('core/Texture')
Texture:include(Resource)

function Texture.static:load( ... )
    return Texture(...)
end

function Texture:initialize( target, fileName, ... )
    if target == '2d' then
        self.handle = NATIVE.Load2dTexture(fileName, ...)
    elseif target == 'cube' then
        self.handle = NATIVE.LoadCubeTexture(fileName, ...)
    else
        error('Unknown type: '..target)
    end
end

function Texture:destroy()
    NATIVE.DestroyTexture(self.handle)
    self.handle = nil
end


return Texture
