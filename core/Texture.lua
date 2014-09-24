local class    = require 'middleclass'
local Resource = require 'apoapsis.core.Resource'
local engine   = require 'apoapsis.engine'
local Load2dTexture   = engine.Load2dTexture
local LoadCubeTexture = engine.LoadCubeTexture
local DestroyTexture  = engine.DestroyTexture


local Texture = class('apoapsis/core/Texture')
Texture:include(Resource)

function Texture.static:load( ... )
    return Texture(...)
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
