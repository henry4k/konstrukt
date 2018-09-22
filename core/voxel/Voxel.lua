--- @classmod core.voxel.Voxel
--- Static world objects, which are made from voxels use this as their base class.


local class = require 'middleclass'


local Voxel = class('core/voxel/Voxel')

-- Voxel._voxelData

local function GatherVoxelClassHieracy( voxelClass, out )
    if voxelClass ~= Voxel then
        GatherVoxelClassHieracy(voxelClass.super, out)
    end
    table.insert(out, voxelClass)
end

function Voxel.static:addAttribute( name, bitCount )
    if not rawget(self.static, '_attributes') then
        self.static._attributes = {}
    end
    table.insert(self.static._attributes, { name = name,
                                            bitCount = bitCount })
end

Voxel.static.idBitCount = 10
Voxel:addAttribute('id', Voxel.static.idBitCount)

function Voxel.static:_createAttributeMap()
    local attributeMap = {}
    local classHieracy = {}
    GatherVoxelClassHieracy(self, classHieracy)
    local bitPosition = 0
    for _, voxelClass in ipairs(classHieracy) do
        for _, attribute in ipairs(voxelClass.static._attributes) do
            assert(not attributeMap[attribute.name],
                   'Attribute has already been defined.')
            attributeMap[attribute.name] = { bitPosition = bitPosition,
                                             bitCount = attribute.bitCount }
            bitPosition = bitPosition + attribute.bitCount
        end
    end
    self._attributeMap = attributeMap
end

function Voxel.static:getAttributeMask( name )
    return self._attributeMap[name]
end

function Voxel.static:_register()
    local dict = require 'core/voxel/VoxelDictionary'
    dict.registerClass(self)
end

function Voxel.static:addVoxelMeshes( meshChunkGenerator )
    -- meant to be overridden in sub classes
end

-- Prepare voxel to be used by voxel volumes.
function Voxel.static:makeInstantiable()
    self:_createAttributeMap()
    self:_register()
end

function Voxel:initialize()
end

function Voxel:getAttribute( name )
    local attribute = self.class._attributeMap[name]
    assert(attribute, 'No such attribute.')
    return self._voxelData:read(attribute.bitPosition,
                                attribute.bitCount)
end

function Voxel:setAttribute( name, value )
    local attribute = self.class._attributeMap[name]
    assert(attribute, 'No such attribute.')
    self._voxelData:write(attribute.bitPosition,
                          attribute.bitCount,
                          value)
end

return Voxel
