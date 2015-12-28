--- @classmod core.graphics.Mesh
--- Geometry that can be directly rendered by the graphics card.
--
-- Meshes are mostly generated from @{core.graphics.MeshBuffer}s.
--
-- Includes @{core.Resource}.


local engine      = require 'engine'
local class       = require 'middleclass'
local Object      = class.Object
local Resource    = require 'core/Resource'
local MeshBuffer  = require 'core/graphics/MeshBuffer'


local Mesh = class('core/graphics/Mesh')
Mesh:include(Resource)

--- Creates a mesh from a mesh buffer.
--
-- @function static:load( ... )
--
-- @param[type=string] ...
-- The same parameters that are valid for mesh buffers.
-- See @{core.graphics.MeshBuffer.static:load}.
--
function Mesh.static:_load( ... )
    local meshBuffer = MeshBuffer:load(...)
    local mesh = Mesh(meshBuffer)
    return { value=mesh, destructor=mesh.destroy }
end

function Mesh:initialize( v )
    if Object.isInstanceOf(v, MeshBuffer) then
        self.handle = engine.CreateMesh(v.handle)
    elseif type(v) == 'userdata' then
        self.handle = v
    else
        error('Must be initialized with a mesh buffer.')
    end
end

function Mesh:destroy()
    engine.DestroyMesh(self.handle)
    self.handle = nil
end


return Mesh
