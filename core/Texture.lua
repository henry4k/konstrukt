local class = require 'core/middleclass.lua'


local Texture = class('core/Texture')

function Texture:initialize( target, fileName, ... )
    if target == '2d' then
        self.handle = NATIVE.Load2dTexture(fileName, ...)
    elseif target == 'cube' then
        self.handle = NATIVE.LoadCubeTexture(fileName, ...)
    else
        error('Unknown type: '..target)
    end
end


return Texture
