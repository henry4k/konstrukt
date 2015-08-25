--- @classmod core.world.Chunk
--
-- Represents a part of the static game world.
-- - contains models, solids, etc. of the static wold in the given area


local format = string.format
local class  = require 'middleclass'
local ChunkBuilder = require 'core/world/ChunkBuilder'


local Chunk = class('core/world/Chunk')

--- Generates a chunk id from chunk coordinates.
function Chunk.static:idFromChunkCoords( x, y, z )
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

--- Updates the chunks world representation
function Chunk:update( voxelVolume, min, max, modelWorld )
    local chunkBuilder = ChunkBuilder()

    local structures = voxelVolume:getStructuresInAABB(min, max)
    for _, structure in ipairs(structures) do
        structure:generateModels(chunkBuilder)
    end

    chunkBuilder:updateModels(self.models, modelWorld)
end


return Chunk
