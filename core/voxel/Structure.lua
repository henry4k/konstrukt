--- @classmod core.voxel.Structure
--- Static world objects, which are made from voxels use this as their base class.
--
-- Extends @{core.world.WorldObject}.


local class         = require 'middleclass'
local VoxelAccessor = require 'core/voxel/VoxelAccessor'


local Structure = class('core/voxel/Structure')

--- Helper function! So you don't need to import the StructureDictionary every time.
function Structure.static:register()
    local dict = require 'core/voxel/StructureDictionary'
    dict.registerClass(self)
end

local voxelAccessor = VoxelAccessor()
voxelAccessor:addMask('id', 10)
Structure.static.voxelAccessor = voxelAccessor

function Structure.static:getOrigin( voxel, position )
    error('Implementation missing.')
end

function Structure:initialize()
end

function Structure:destroy()
end

function Structure:readVoxel( position )
    assert(self._mayReadVoxels, 'Reading not allowed.')
    if self:ownsVoxel(position) then
        local voxel = self._voxelVolume:readVoxel(position)
        if voxel then
            return voxel
        else
            error('Can\'t read voxel at '..tostring(position)..'.')
        end
    else
        error('Can\'t read voxel at '..tostring(position)..', as it doesn\'t belong to the structure.')
    end
end

function Structure:writeVoxel( position, voxel )
    assert(self._mayWriteVoxels, 'Writing not allowed.')
    if self:ownsVoxel(position) then
        local oldStructure = self._voxelVolume:getStructureAt(position)
        if oldStructure and oldStructure ~= self then
            -- TODO: Inform structure, which intersects the new area, about its destruction.
            print(string.format('TODO: Replaced structure %s should be destroyed.', oldStructure.class.name))
        end

        if not self._voxelVolume:writeVoxel(position, voxel) then
            error('Can\'t write voxel at '..tostring(position)..'.')
        end
    else
        error('Can\'t write voxel at '..tostring(position)..', as it doesn\'t belong to the structure.')
    end
end

function Structure:_create( voxelVolume, origin, ... )
    self.origin = origin
    self:create(...)
    self:_write(voxelVolume)
end

function Structure:_read( voxelVolume, origin )
    self.origin = origin
    self._voxelVolume = voxelVolume
    self._mayReadVoxels = true
    self:read()
    self._mayReadVoxels = false
end

function Structure:_write( voxelVolume )
    self._voxelVolume = voxelVolume
    self._mayWriteVoxels = true
    self:write()
    self._mayWriteVoxels = false
end

function Structure:ownsVoxel( position )
    error('Implementation missing.')
end

function Structure:create()
    -- Dummy function.  This is meant to be overridden in child classes.
end

function Structure:read( voxelReader )
    -- Dummy function.  This is meant to be overridden in child classes.
end

function Structure:write( voxelWriter )
    -- Dummy function.  This is meant to be overridden in child classes.
end


return Structure
