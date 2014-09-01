local class = require 'core/middleclass'
local Resource = require 'core/Resource'


local Mesh = class('core/Mesh')
Mesh:include(Resource)

function Mesh.static:load( sceneFileName, objectName )
    local scene = Json.decodeFromFile(sceneFileName)
    if scene then
        local meshBuffer = Scene.createMeshBufferByPath(scene, objectName)
        return Mesh(meshBuffer)
    else
        return nil
    end
end

function Mesh:initialize( meshBuffer )
    self.handle = NATIVE.CreateMesh(meshBuffer.handle)
end

function Mesh:destroy()
    NATIVE.DestroyMesh(self.handle)
    self.handle = nil
end


return Mesh
