--- @classmod core.voxel.VoxelEntity


local class        = require 'middleclass'
local Voxel        = require 'core/voxel/Voxel'
local EffectTarget = require 'core/EffectTarget'
local Entity       = require 'core/world/Entity'


local VoxelEntity = class('core/voxel/VoxelEntity', Voxel)
VoxelEntity:include(EffectTarget)
VoxelEntity:include(Entity)

function VoxelEntity:initialize()
    Voxel.initialize(self)
    self:initializeEffectTarget()
    self:initializeEntity()
    self.attachmentTarget = nil -- TODO: Attach to the chunks solid
end

function VoxelEntity:destroy()
    --Voxel.destroy() - Doesn't exist
    self:destroyEntity()
    self:destroyEffectTarget()
end

return VoxelEntity
