local class         = require 'middleclass'
local Vec           = require 'core/Vector'
local Structure     = require 'core/world/Structure'
local Voxel         = require 'core/world/Voxel'
local VoxelAccessor = require 'core/world/VoxelAccessor'


-- AabbStructures have an origin point and a size (this is called AABB).
-- This defines which voxel belong to the structure.
local AabbStructure = class('core/world/AabbStructure', Structure)

local minSize = Vec(1, 1, 1)
local maxSize = Vec(15, 15, 15) -- 2^4 - 1

local voxelAccessor = VoxelAccessor(Structure.voxelAccessor)
local startPosition = Structure.voxelAccessor.nextPosition
voxelAccessor:addMask('isOrigin', 1)
voxelAccessor:addMask('sizeX', 4)
voxelAccessor:addMask('sizeY', 4)
voxelAccessor:addMask('sizeZ', 4)
voxelAccessor:setMask('originOffsetX', startPosition + 1 + 0, 4)
voxelAccessor:setMask('originOffsetY', startPosition + 1 + 4, 4)
voxelAccessor:setMask('originOffsetZ', startPosition + 1 + 8, 4)
AabbStructure.static.voxelAccessor = voxelAccessor

function AabbStructure.static:getOrigin( voxel, position )
    if voxelAccessor:read(voxel, 'isOrigin') then
        return position
    else
        local offset = Vec(voxelAccessor:read(voxel, 'originOffsetX'),
                           voxelAccessor:read(voxel, 'originOffsetY'),
                           voxelAccessor:read(voxel, 'originOffsetZ'))
        return position - offset
    end
end

function AabbStructure:initialize( ... )
    Structure.initialize(self, ...)
    self.size = nil
end

function AabbStructure:destroy()
    Structure.destroy()
    -- ...
end

function AabbStructure:ownsVoxel( position )
    return position:componentsGreaterOrEqualTo(self.origin) and
           position:componentsLesserThan(self.origin+self.size)
end

function AabbStructure:create( voxelCreator, size )
    assert(size:componentsGreaterOrEqualTo(minSize) and
           size:componentsLesserOrEqualTo(maxSize))
    self.size = size

    local originVoxel = Voxel()
    voxelAccessor:write(originVoxel, 'isOrigin', 1)
    voxelAccessor:write(originVoxel, 'sizeX', size[1])
    voxelAccessor:write(originVoxel, 'sizeY', size[2])
    voxelAccessor:write(originVoxel, 'sizeZ', size[3])
    voxelCreator:writeVoxel(self.origin, originVoxel)

    self:clear(voxelCreator)
end

function AabbStructure:read( voxelReader )

end

function AabbStructure:write( voxelWriter )

end

function AabbStructure:clear( voxelCreator )
    local size = self.size
    local origin = self.origin

    local emptyVoxel = Voxel()
    voxelAccessor:write(emptyVoxel, 'isOrigin', 0)

    -- start with z = 1 as the origin voxel must be preserved
    for offsetZ = 1, size[3]-1 do
    for offsetY = 0, size[2]-1 do
    for offsetX = 0, size[1]-1 do
        voxelAccessor:write(emptyVoxel, 'originOffsetX', offsetX)
        voxelAccessor:write(emptyVoxel, 'originOffsetY', offsetY)
        voxelAccessor:write(emptyVoxel, 'originOffsetZ', offsetZ)
        local offset = Vec(offsetX, offsetY, offsetZ)
        voxelCreator:writeVoxel(origin + offset, emptyVoxel)
    end
    end
    end
end

function AabbStructure:write( voxelWriter )

end

function AabbStructure:read( voxelReader )
    self.size = Vec(1,1,1) -- so at least the origin voxel can be accessed
    local originVoxel = voxelReader:readVoxel(self.origin)
    self.size = Vec(voxelAccessor:read(originVoxel, 'sizeX'),
                    voxelAccessor:read(originVoxel, 'sizeY'),
                    voxelAccessor:read(originVoxel, 'sizeZ'))
    assert(self.size:componentsGreaterOrEqualTo(minSize) and
           self.size:componentsLesserOrEqualto(maxSize))
end


return AabbStructure
