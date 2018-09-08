local class             = require 'middleclass'
local Object            = class.Object
local Voxel             = require 'core/voxel/Voxel'
local UndefinedVoxel    = require 'core/voxel/UndefinedVoxel'
local GlobalEventSource = require 'core/GlobalEventSource'


local VoxelDictionary = {
    classes = {},
    idToClass = nil
}

function VoxelDictionary.registerClass( voxelClass )
    assert(Object.isSubclassOf(voxelClass, Voxel),
           'Voxel classes must inherit the voxel base class.')
    assert(not VoxelDictionary.idToClass,
           'Voxel classes must be registered before assigning IDs to them.')
    table.insert(VoxelDictionary.classes, voxelClass)
end

--- Work out correct IDs for registered voxel classes.
-- Must be called only *once* after all voxel classes have been registered.
function VoxelDictionary._assignIds()
    assert(not VoxelDictionary.idToClass,
           'Voxel class IDs have already been assigned.')
    local idToClass = {}

    idToClass[0] = UndefinedVoxel

    -- For now, this will just be implemented super simple:
    for i, v in ipairs(VoxelDictionary.classes) do
        idToClass[i] = v
        v.static.id = i
    end

    VoxelDictionary.idToClass = idToClass
end

function VoxelDictionary.getClassFromId( id )
    local idToClass = VoxelDictionary.idToClass
    assert(idToClass, 'VoxelDictionary.assignIds() was not called yet.')
    local voxelClass = idToClass[id]
    assert(voxelClass, 'No voxel class matches!')
    return voxelClass
end

GlobalEventSource:addEventTarget('resources loaded', VoxelDictionary, function()
    VoxelDictionary._assignIds()
end)


return VoxelDictionary
