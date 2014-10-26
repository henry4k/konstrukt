local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Mat4   = require 'core/Matrix4'
local CreateMeshBuffer         = ENGINE.CreateMeshBuffer
local DestroyMeshBuffer        = ENGINE.DestroyMeshBuffer
local TransformMeshBuffer      = ENGINE.TransformMeshBuffer
local AppendMeshBuffer         = ENGINE.AppendMeshBuffer
local AppendIndexToMeshBuffer  = ENGINE.AppendIndexToMeshBuffer
local AppendVertexToMeshBuffer = ENGINE.AppendVertexToMeshBuffer


local MeshBuffer = class('core/graphics/MeshBuffer')

function MeshBuffer:initialize()
    self.handle = CreateMeshBuffer()
end

function MeshBuffer:destroy()
    DestroyMeshBuffer(self.handle)
    self.handle = nil
end

function MeshBuffer:transform( transformation )
    assert(Object.isInstanceOf(transformation, Mat4), 'Transformation must be an matrix.')
    TransformMeshBuffer(self.handle, transformation.handle)
end

function MeshBuffer:appendMeshBuffer( other, transformation )
    assert(Object.isInstanceOf(other, MeshBuffer), 'Must be called with another mesh buffer.')
    assert(Object.isInstanceOf(transformation, Mat4), 'Transformation must be an matrix.')
    if transformation then
        AppendMeshBuffer(self.handle, other.handle, transformation.handle)
    else
        AppendMeshBuffer(self.handle, other.handle)
    end
end

function MeshBuffer:appendIndex( index )
    assert(index >= 0, 'Index must be positive.')
    AppendIndexToMeshBuffer(self.handle, index)
end

function MeshBuffer:appendVertex( vertex )
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


return MeshBuffer
