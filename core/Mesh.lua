local class = require 'core/middleclass'
local ResourceManager = require 'core/ResourceManager'


local GetEntryByPath = function( table, path, delimiters )
    local delimiter = delimiter or '.'
    local current = table
    for entry in path:gmatch('[^'..delimiters..']+') do
        if current[entry] then
            current = current[entry]
        else
            return nil
        end
    end
    return current
end


local Mesh = class('core/Mesh')

function Mesh.static:registerResource()
    ResourceManager.registerLoader('core/Mesh', function( sceneFileName, objectName )
        local scene = Json.decodeFromFile(sceneFileName)
        if scene then
            local meshData = GetEntryByPath(scene, objectName, './')
            local meshBuffer = Scene.createMeshBuffer(meshData)
            return Mesh:new(meshBuffer)
        else
            return nil
        end
    end)
end

function Mesh:initialize( meshBuffer )
    self.handle = NATIVE.CreateMesh(meshBuffer.handle)
end

function Mesh:destroy()
    NATIVE.DestroyMesh(self.handle)
    self.handle = nil
end


return Mesh
