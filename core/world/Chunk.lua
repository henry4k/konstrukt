local format = string.format
local class  = require 'middleclass'


--- Represents a part of the static game world.
local Chunk = class('core/world/Chunk')

--- Generate a chunk id from chunk coordinates.
-- Don't use world coordinates here!
function Chunk.static:generateId( x, y, z )
    return format('%d%d%d', x, y, z)
end

function Chunk:initialize()
    self.models = {}
end

function Chunk:destroy()
    -- destroy created resource here (meshes and models probably)
    for _, model in pairs(self.models) do
        model:destroy()
    end
    self.models = nil
end

function Chunk:update( voxelVolume, min, max, modelWorld )
    local chunkBuilder = ChunkBuilder()

    local structures = voxelVolume:getStructuresInAABB(min, max)
    for _, structure in ipairs(structures) do
        structure:generateModels(chunkBuilder)
    end

    chunkBuilder:updateModels(self.models, modelWorld)
end


return Chunk
