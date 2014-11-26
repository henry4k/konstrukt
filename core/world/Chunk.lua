local format  = string.format
local class   = require 'middleclass'
local Object  = class.Object
local Vec     = require 'core/Vector'


--- Represents a part of the static game world.
local Chunk = class('core/world/Chunk')

function Chunk.static:generateId( x, y, z )
    return format('%d%d%d', x, y, z)
end

function Chunk:initialize()
    -- store textures, meshes, models and shaders here
end

function Chunk:destroy()
end

function Chunk:update( voxelVolume, min, max )
end


return Chunk
