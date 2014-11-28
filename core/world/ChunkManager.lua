local floor       = math.floor
local ceil        = math.ceil
local class       = require 'middleclass'
local Object      = class.Object
local Vec         = require 'core/Vector'
local Chunk       = require 'core/world/Chunk'
local VoxelVolume = require 'core/voxel/VoxelVolume'


--- Manages the static game world, by dividing it into chunks.
local ChunkManager = class('core/world/ChunkManager')

function ChunkManager:initialize( voxelVolume )
    assert(Object.isInstanceOf(voxelVolume, VoxelVolume),
           'Must be initialized with a voxel volume.')
    self.voxelVolume = voxelVolume
    self.chunkSize = 8
    self.chunks = {}
    self.activators = {} -- Defines which chunks need to be active.
end

function ChunkManager:destroy()
    for _, chunk in pairs(self.chunks) do
        chunk:destroy()
    end
    self.chunks = nil
end

function ChunkManager:addActivator( activator )
    table.insert(self.activators, activator)
end

function ChunkManager:removeActivator( activator )
    local activators = self.activators
    local i = 1
    while i <= #activators do
        if activators[i] == activator then
            table.remove(activators, i)
        else
            i = i + 1
        end
    end
end

--- Create needed chunks and destroys unneeded ones.
-- Should be called in regular intervals, but not too frequent since creating
-- and destroying chunks can be quite slow.
function ChunkManager:update()
    -- Work out which chunks are needed:
    local neededChunks = {}
    for _, activator in ipairs(self.activators) do
        local position = activator:getPosition()
        local range    = activator:getRange()

        local halfWidths = Vec(range, range, range)
        local minVoxel = position - halfWidths
        local maxVoxel = position + halfWidths
        local minChunk, maxChunk = self:_voxelToChunkRange(minVoxel, maxVoxel)
        print(string.format('Needs voxels from %s to %s', minVoxel, maxVoxel))
        print(string.format('Needs chunks from %s to %s', minChunk, maxChunk))

        for z = minChunk[3], maxChunk[3], 1 do
        for y = minChunk[2], maxChunk[2], 1 do
        for x = minChunk[1], maxChunk[1], 1 do
            local id = Chunk:generateId(x, y, z)
            neededChunks[id] = Vec(x, y, z)
        end
        end
        end
    end

    local chunks = self.chunks

    -- Destroy unneeded chunks:
    for chunkId, chunk in pairs(chunks) do
        if not neededChunks[chunkId] then
            chunk:destroy()
            chunks[chunkId] = nil
        end
    end

    -- Create needed chunks:
    local chunkHalfWidths = Vec(1, 1, 1) * (self.chunkSize / 2)
    for chunkId, chunkPosition in pairs(neededChunks) do
        if not chunks[chunkId] then
            local chunk = Chunk()
            chunks[chunkId] = chunk
            local min = chunkPosition - chunkHalfWidths
            local max = chunkPosition + chunkHalfWidths
            chunk:update(self.voxelVolume, min, max)
        end
    end
end

function ChunkManager:_voxelToChunkRange( minVoxel, maxVoxel )
    return self:voxelToChunkCoordinates(minVoxel),
           self:voxelToChunkCoordinates(maxVoxel)
end

function ChunkManager:_voxelToChunkCoordinates( x, y, z )
    local chunkSize = self.chunkSize
    return floor(x / chunkSize),
           floor(y / chunkSize),
           floor(z / chunkSize)
end

--- Chunk position, which covers the world at the given coordinates.
function ChunkManager:voxelToChunkCoordinates( voxelCoords )
    assert(Vec:isInstance(voxelCoords), 'Voxel coordinates must be a vector.')
    return Vec(self:_voxelToChunkCoordinates(voxelCoords:unpack(3)))
end

--- Tries to retrieve the active chunk that covers the given world position.
-- Returns `nil` if the at that position is not active.
function ChunkManager:getActiveChunkAt( voxelCoords )
    assert(Vec:isInstance(voxelCoords), 'Voxel coordinates must be a vector.')
    local x, y, z = self:_voxelToChunkCoordinates(voxelCoords:unpack(3))
    local id = Chunk:generateId(x, y, z)
    return self.chunks[id]
end


return ChunkManager
