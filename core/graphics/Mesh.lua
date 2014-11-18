local assert      = assert
local class       = require 'middleclass'
local Object      = class.Object
local Resource = require 'core/Resource'
local MeshBuffer  = require 'core/graphics/MeshBuffer'
local CreateMesh  = ENGINE.CreateMesh
local DestroyMesh = ENGINE.DestroyMesh


local Mesh = class('core/graphics/Mesh')
Mesh:include(Resource)

function Mesh.static:_load( ... )
    local meshBuffer = MeshBuffer:load(...)
    local mesh = Mesh(meshBuffer)
    return { value=mesh, destructor=mesh.destroy }
end

function Mesh:initialize( meshBuffer )
    assert(Object.isInstanceOf(meshBuffer, MeshBuffer),
           'Must be initialized with a mesh buffer.')
    self.handle = CreateMesh(meshBuffer.handle)
end

function Mesh:destroy()
    DestroyMesh(self.handle)
    self.handle = nil
end


return Mesh
