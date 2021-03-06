--- @classmod core.graphics.MeshBuffer
--- Used to store and assemble @{Mesh}es.
--
-- Includes @{core.Resource}.


local engine    = require 'engine'
local class     = require 'middleclass'
local Object    = class.Object
local Scheduler = require 'core/Scheduler'
local Mat4      = require 'core/Matrix4'
local Json      = require 'core/Json'
local Resource  = require 'core/Resource'


local function GetEntryByPath( table, path, delimiters )
    delimiters = delimiters or '.'
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


local MeshBuffer = class('core/graphics/MeshBuffer')
MeshBuffer:include(Resource)

--- Loads a mesh buffer from a JSON encoded scene.
--
-- @function static:load( sceneFileName, objectName )
--
-- @usage
-- doorMeshBuffer = MeshBuffer:load('airlock.json', 'airlock/door')
--
function MeshBuffer.static:_load( sceneFileName, objectPath )
    local sceneGraph = Json.decodeFromFile(sceneFileName)
    if sceneGraph then
        local definition = GetEntryByPath(sceneGraph, objectPath, './')
        if not definition then
            error(('%s not found in %s'):format(objectPath, sceneFileName))
        end
        local meshBuffer = MeshBuffer()
        meshBuffer:readDefinition(definition)
        meshBuffer:lock()
        return { value=meshBuffer, destructor=meshBuffer.destroy }
    else
        return nil
    end
end

function MeshBuffer:initialize()
    self.handle = Scheduler.awaitCall(engine.CreateMeshBuffer)
    self.locked = false
end

function MeshBuffer:destroy()
    Scheduler.blindCall(engine.DestroyMeshBuffer, self.handle)
    self.handle = nil
end

--- Whether the buffer is write protected.
-- @see lock
function MeshBuffer:isLocked()
    return self.locked
end

--- Protect the buffer from being modified.
-- @see isLocked
function MeshBuffer:lock()
    self.locked = true
end

--- Transform the vertices by an transformation matrix.
function MeshBuffer:transform( transformation )
    assert(not self.locked, 'Mesh buffer is write protected.')
    assert(Object.isInstanceOf(transformation, Mat4), 'Transformation must be an matrix.')
    Scheduler.blindCall(engine.TransformMeshBuffer, self.handle, transformation.handle)
end

--- Build index buffer from scratch.
function MeshBuffer:index()
    assert(not self.locked, 'Mesh buffer is write protected.')
    Scheduler.blindCall(engine.IndexMeshBuffer, self.handle)
end

--- Calculate normals from scratch.
function MeshBuffer:calcNormals()
    assert(not self.locked, 'Mesh buffer is write protected.')
    Scheduler.blindCall(engine.CalcMeshBufferNormals, self.handle)
end

--- Calculate tangents from scratch.
-- Make sure that the mesh has normals and texture coordinates.
function MeshBuffer:calcTangents()
    assert(not self.locked, 'Mesh buffer is write protected.')
    Scheduler.blindCall(engine.CalcMeshBufferTangents, self.handle)
end

--- Append vertices from another buffer.
--
-- @param[type=MeshBuffer] other
--
-- @param[type=Matrix4,opt] transformation
-- Transformation that is (optionally) applied to the appended vertices.
--
function MeshBuffer:appendMeshBuffer( other, transformation )
    assert(not self.locked, 'Mesh buffer is write protected.')
    assert(Object.isInstanceOf(other, MeshBuffer), 'Must be called with another mesh buffer.')
    if transformation then
        assert(Object.isInstanceOf(transformation, Mat4), 'Transformation must be an matrix.')
        Scheduler.blindCall(engine.AppendMeshBuffer, self.handle, other.handle, transformation.handle)
    else
        Scheduler.blindCall(engine.AppendMeshBuffer, self.handle, other.handle)
    end
end

--- Add a new index.
-- See the [OpenGL documentation on index buffers](https://www.opengl.org/wiki/Vertex_Specification#Index_buffers).
function MeshBuffer:appendIndex( index )
    assert(not self.locked, 'Mesh buffer is write protected.')
    assert(index >= 0, 'Index must be positive.')
    Scheduler.blindCall(engine.AppendIndexToMeshBuffer, self.handle, index)
end

--- Add a new vertex.
--
-- @param[type=table] vertex
-- Format:
--
-- - `x, y, z`: position (mandatory)
-- - `r, g, b`: color (optional; defaults to 1,1,1)
-- - `tx, ty`: texture coordinate (optional; defaults to 0,0)
-- - `nx, ny, nz`: normal vector (optional; defaults to 0,1,0)
-- - `tanx, tany, tanz`: tangent vector (optional; defaults to 1,0,0,1)
--
function MeshBuffer:appendVertex( vertex )
    assert(not self.locked, 'Mesh buffer is write protected.')
    local v = vertex
    Scheduler.blindCall(engine.AppendVertexToMeshBuffer,
        self.handle,

        -- position
        v.x,
        v.y,
        v.z,

        -- color
        v.r or 0,
        v.g or 0,
        v.b or 0,

        -- texture coord
        v.tx or 0,
        v.ty or 0,

        -- normal
        v.nx or 0,
        v.ny or 1,
        v.nz or 0,

        -- tangent
        v.tanx or 1,
        v.tany or 0,
        v.tanz or 0,
        v.tanw or 1
    )
end

function MeshBuffer:readDefinition( definition )
    assert(definition.vertices)
    assert(definition.faces)

    for _,vertex in ipairs(definition.vertices) do
        self:appendVertex(vertex)
    end

    for _,face in ipairs(definition.faces) do
        assert(#face == 3, 'Only triangles are supported!')
        for _,index in ipairs(face) do
            self:appendIndex(index)
        end
    end

    self:calcTangents()

    return self
end


return MeshBuffer
