local format  = string.format
local class   = require 'middleclass'
local Object  = class.Object
local Vec     = require 'core/Vector'


--- Represents a part of the static game world.
local Chunk = class('core/world/Chunk')

--- Generate a chunk id from chunk coordinates.
-- Don't use world coordinates here!
function Chunk.static:generateId( x, y, z )
    return format('%d%d%d', x, y, z)
end

function Chunk:initialize()
    -- store needed textures and shaders here
    -- create models
    -- create meshes
end

function Chunk:destroy()
    -- destroy created resource here (meshes and models probably)
end

function Chunk:update( voxelVolume, min, max )
    -- update meshes
    --[[
    for z = min[3], max[3], 1 do
    for y = min[2], max[2], 1 do
    for x = min[1], max[1], 1 do
        -- TODO: Update stuff here!
    end
    end
    end
    ]]
end


return Chunk
