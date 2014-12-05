local tostring  = tostring
local class     = require 'middleclass'
local VoxelVolume = require 'core/world/VoxelVolume'


local CachingVoxelVolume = class('core/world/CachingVoxelVolume', VoxelVolume)

function CachingVoxelVolume:initialize( size )
    VoxelVolume.initialize(size)
    self.readCache = {}
    self.writeCache = {}
end

function CachingVoxelVolume:destroy()
    VoxelVolume.destroy(self)
end

function CachingVoxelVolume:readVoxel( position )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    local hash = tostring(position)
    local readCache = self.readCache
    local voxel = readCache[hash]
    if not voxel then
        voxel = VoxelVolume.readVoxel(self, position)
        readCache[hash] = voxel
    end
    return voxel
end

function CachingVoxelVolume:writeVoxel( position, voxel )
    assert(Vec:isInstance(position), 'Position must be a vector.')
    assert(Voxel:isInstance(voxel), 'Must be called with a voxel.')
    local hash = tostring(position)
    self.readCache[hash] = voxel
    self.writeCache[hash] = { position=position, voxel=voxel }
end

function CachingVoxelVolume:flushCaches()
    VoxelVolume:flushCaches()

    for hash, data in pairs(self.writeCache) do
        VoxelVolume.writeVoxel(self, data.position, data.voxel)
    end
    self.readCache = {}
    self.writeCache = {}
end
