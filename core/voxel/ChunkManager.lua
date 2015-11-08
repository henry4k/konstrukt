local assert      = assert
local floor       = math.floor
local class       = require 'middleclass'
local Object      = class.Object
local Vec         = require 'core/Vector'
local Chunk       = require 'core/voxel/Chunk'
local VoxelVolume = require 'core/voxel/VoxelVolume'
local ModelWorld  = require 'core/graphics/ModelWorld'


--- Manages the static game world, by dividing it into chunks.
local ChunkManager = class('core/voxel/ChunkManager')

function ChunkManager:initialize( voxelVolume, modelWorld )
    assert(Object.isInstanceOf(voxelVolume, VoxelVolume),
           'Must be initialized with a voxel volume.')
    assert(Object.isInstanceOf(modelWorld, ModelWorld),
           'Must be initialized with a model world.')
    self.voxelVolume = voxelVolume
    self.modelWorld = modelWorld
    self.chunkSize = 8
    self.chunks = {}
    self.activators = {} -- Defines which chunks need to be active.
    self.modifiedChunks = {}

    voxelVolume:addEventTarget('voxel-modified', self, self.onVoxelModification)
end

function ChunkManager:destroy()
    self.voxelVolume:removeEventTarget('voxel-modified', self)
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
    -- TODO: Enhance function behaviour
end

function ChunkManager:onVoxelModification( position )
    local x, y, z = self:_voxelToChunkCoordinates(position[1],
                                                  position[2],
                                                  position[3])
    local id = Chunk:idFromChunkCoords(x, y, z)
    self.modifiedChunks[id] = Vec(x, y, z)
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
            local id = Chunk:idFromChunkCoords(x, y, z)
            neededChunks[id] = Vec(x, y, z)
        end
        end
        end
    end

    local chunks = self.chunks

    -- Destroy unneeded chunks:
    for chunkId, chunk in pairs(chunks) do
        if not neededChunks[chunkId] then
            print(string.format('Chunk %s not needed anymore - destroying it.', chunkId))
            chunk:destroy()
            chunks[chunkId] = nil
        end
    end

    -- Create needed chunks:
    local modifiedChunks = self.modifiedChunks
    local modelWorld = self.modelWorld
    for chunkId, chunkPosition in pairs(neededChunks) do
        if not chunks[chunkId] then
            local chunk = Chunk()
            chunks[chunkId] = chunk
            modifiedChunks[chunkId] = chunkPosition
        end
    end

    -- Update modified chunks:
    for chunkId, chunkPosition in pairs(modifiedChunks) do
        local chunk = chunks[chunkId]
        if chunk then
            local voxelPosition = self:chunkToVoxelCoordinates(chunkPosition)
            local min = voxelPosition
            local max = voxelPosition + (self.chunkSize-1)
            print(string.format('Updating chunk %s from %s to %s.', chunkId, min, max))
            chunk:update(self.voxelVolume, min, max, modelWorld)
        end
    end
    self.modifiedChunks = {}
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

function ChunkManager:_chunkToVoxelCoordinates( x, y, z )
    local chunkSize = self.chunkSize
    return x * chunkSize,
           y * chunkSize,
           z * chunkSize
end

--- Chunk position, which covers the world at the given coordinates.
function ChunkManager:chunkToVoxelCoordinates( chunkCoords )
    assert(Vec:isInstance(chunkCoords), 'Chunk coordinates must be a vector.')
    return Vec(self:_chunkToVoxelCoordinates(chunkCoords:unpack(3)))
end

--- Tries to retrieve the active chunk that covers the given world position.
-- Returns `nil` if the at that position is not active.
function ChunkManager:getActiveChunkAt( voxelCoords )
    assert(Vec:isInstance(voxelCoords), 'Voxel coordinates must be a vector.')
    local x, y, z = self:_voxelToChunkCoordinates(voxelCoords:unpack(3))
    local id = Chunk:idFromChunkCoords(x, y, z)
    return self.chunks[id]
end


return ChunkManager
