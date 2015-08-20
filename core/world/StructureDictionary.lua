local class     = require 'middleclass'
local Object    = class.Object
local Structure = require 'core/world/Structure'
local UndefinedStructure = require 'core/world/UndefinedStructure'


local StructureDictionary = {
    classes = {},
    idToClass = nil
}

function StructureDictionary.registerClass( structureClass )
    assert(Object.isSubclassOf(structureClass, Structure),
           'Structure classes must inherit the structure base class.')
    assert(not StructureDictionary.idToClass,
           'Structure classes must be registered before assigning IDs to them.')
    table.insert(StructureDictionary.classes, structureClass)
end

--- Work out correct IDs for registered structure classes.
-- Must be called only *once* after all structure classes have been registered.
function StructureDictionary.assignIds()
    assert(not StructureDictionary.idToClass,
           'Structure class IDs have already been assigned.')
    local idToClass = {}

    idToClass[0] = UndefinedStructure

    -- For now, this will just be implemented super simple:
    for i, v in ipairs(StructureDictionary.classes) do
        local id = i - 1
        idToClass[id] = v
        v.static.id = id
    end

    StructureDictionary.idToClass = idToClass
end

function StructureDictionary.getClassFromId( id )
    local idToClass = StructureDictionary.idToClass
    assert(idToClass, 'StructureDictionary.assignIds() was not called yet.')
    local structureClass = idToClass[id]
    assert(structureClass, 'No structure class matches!')
    return structureClass
end


return StructureDictionary
