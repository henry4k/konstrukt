local class       = require 'middleclass'
local Object      = class.Object
local Vec         = require 'core/Vector'
local Volume      = require 'core/Volume'
local Chunk       = require 'core/world/Chunk'
local VoxelVolume = require 'core/world/VoxelVolume'


--- Manages the static game world, by dividing it into chunks.
-- Only chunks
--
local ChunkManager = class('core/world/ChunkManager')

function ChunkManager:initialize( voxelVolume )
    assert(Object.isInstanceOf(voxelVolume, VoxelVolume),
           'Must be initialized with a voxel volume.')
    self.voxelVolume = voxelVolume
    self.chunks = {}
    self.activeVolumes = {} -- Defines which chunks need to be active.
end

function ChunkManager:destroy()
end

function ChunkManager:addActiveVolume( volume )
    assert(Object.isInstanceOf(volume, Volume), 'Must be called with a volume.')
    table.insert(self.activeVolumes, volume)
end

function ChunkManager:removeActiveVolume( volume )
    assert(Object.isInstanceOf(volume, Volume), 'Must be called with a volume.')
    local activeVolumes = self.activeVolumes
    local i = 1
    while i < #activeVolumes do
        if activeVolumes[i] == volume then
            table.remove(activeVolumes, i)
        else
            i = i + 1
        end
    end
end

--- Create needed chunks, destroys unused chunks and updates existing ones.
function ChunkManager:update()
    -- loop through each loaded chunk and test if its at least in one volume
end

function ChunkManager:getChunkAt( x, y, z )
    local id = Chunk:generateId(x, y, z)
    return self.chunks[id]
end


return ChunkManager
