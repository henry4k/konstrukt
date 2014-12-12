---
-- @module core.graphics.MeshBuffer
-- @see Mesh


local assert   = assert
local class    = require 'middleclass'
local Object   = class.Object
local Mat4     = require 'core/Matrix4'
local Json     = require 'core/Json'
local Resource = require 'core/Resource'
local CreateMeshBuffer         = ENGINE.CreateMeshBuffer
local DestroyMeshBuffer        = ENGINE.DestroyMeshBuffer
local TransformMeshBuffer      = ENGINE.TransformMeshBuffer
local AppendMeshBuffer         = ENGINE.AppendMeshBuffer
local AppendIndexToMeshBuffer  = ENGINE.AppendIndexToMeshBuffer
local AppendVertexToMeshBuffer = ENGINE.AppendVertexToMeshBuffer


local GetEntryByPath = function( table, path, delimiters )
    local delimiters = delimiters or '.'
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

function MeshBuffer.static:_load( sceneFileName, objectName )
    local sceneGraph = Json.decodeFromFile(sceneFileName)
    if sceneGraph then
        local definition = GetEntryByPath(sceneGraph, objectName, './')
        local meshBuffer = MeshBuffer()
        meshBuffer:readDefinition(definition)
        meshBuffer:lock()
        return { value=meshBuffer, destructor=meshBuffer.destroy }
    else
        return nil
    end
end

function MeshBuffer:initialize()
    self.handle = CreateMeshBuffer()
    self.locked = false
end

function MeshBuffer:destroy()
    DestroyMeshBuffer(self.handle)
    self.handle = nil
end

function MeshBuffer:isLocked()
    return self.locked
end

function MeshBuffer:lock()
    self.locked = true
end

function MeshBuffer:transform( transformation )
    assert(not self.locked, 'Mesh buffer is write protected.')
    assert(Object.isInstanceOf(transformation, Mat4), 'Transformation must be an matrix.')
    TransformMeshBuffer(self.handle, transformation.handle)
end

function MeshBuffer:appendMeshBuffer( other, transformation )
    assert(not self.locked, 'Mesh buffer is write protected.')
    assert(Object.isInstanceOf(other, MeshBuffer), 'Must be called with another mesh buffer.')
    if transformation then
        assert(Object.isInstanceOf(transformation, Mat4), 'Transformation must be an matrix.')
        AppendMeshBuffer(self.handle, other.handle, transformation.handle)
    else
        AppendMeshBuffer(self.handle, other.handle)
    end
end

function MeshBuffer:appendIndex( index )
    assert(not self.locked, 'Mesh buffer is write protected.')
    assert(index >= 0, 'Index must be positive.')
    AppendIndexToMeshBuffer(self.handle, index)
end

function MeshBuffer:appendVertex( vertex )
    assert(not self.locked, 'Mesh buffer is write protected.')
    local v = vertex
    AppendVertexToMeshBuffer(
        self.handle,

        -- position
        v.x,
        v.y,
        v.z,

        -- color
        v.r or 1,
        v.g or 1,
        v.b or 1,

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

    return self
end


return MeshBuffer
