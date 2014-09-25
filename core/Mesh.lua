local class    = require 'middleclass'
local Json     = require 'core/Json'
local Scene    = require 'core/Scene'
local Resource = require 'core/Resource'
local CreateMesh  = ENGINE.CreateMesh
local DestroyMesh = ENGINE.DestroyMesh


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
    self.handle = CreateMesh(meshBuffer.handle)
end

function Mesh:destroy()
    DestroyMesh(self.handle)
    self.handle = nil
end


return Mesh
