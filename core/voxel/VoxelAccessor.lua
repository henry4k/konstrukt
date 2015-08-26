local assert = assert
local class  = require 'middleclass'
local Object = class.Object


local VoxelAccessor = class('core/voxel/VoxelAccessor')

function VoxelAccessor:initialize( other )
    assert(not other or Object.isInstanceOf(other, VoxelAccessor),
           'Must be called without parameters or with another voxel accessor.')

    self.masks = {}
    self.nextPosition = 1

    if other then
        setmetatable(self.masks, {__index=other.masks})
        self.nextPosition = other.nextPosition
    end
end

function VoxelAccessor:setMask( name, bitPosition, bitCount )
    assert(not self.masks[name], 'A mask called "'..name..'" has already been set.')
    self.masks[name] = { bitPosition=bitPosition, bitCount=bitCount }
    return self
end

function VoxelAccessor:addMask( name, bitCount )
    self:setMask(name, self.nextPosition, bitCount)
    self.nextPosition = self.nextPosition + bitCount
    return self
end

function VoxelAccessor:read( voxel, name )
    local mask = self.masks[name]
    assert(mask, 'Requested mask not defined in this accessor!')
    return voxel:read(mask.bitPosition, mask.bitCount)
end

function VoxelAccessor:write( voxel, name, value )
    local mask = self.masks[name]
    assert(mask, 'Requested mask not defined in this accessor!')
    return voxel:write(mask.bitPosition, mask.bitCount, value)
end


return VoxelAccessor
