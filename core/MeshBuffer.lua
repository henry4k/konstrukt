local class = require 'core/middleclass'


local MeshBuffer = class('core/MeshBuffer')

function MeshBuffer:initialize()
    self.handle = NATIVE.CreateMeshBuffer()
end

function MeshBuffer:destroy()
    NATIVE.DestroyMeshBuffer(self.handle)
    self.handle = nil
end

function MeshBuffer:transform( transformation )
    NATIVE.TransformMeshBuffer(self.handle, transformation.handle)
end

function MeshBuffer:appendMeshBuffer( other, transformation )
    if transformation then
        NATIVE.AppendMeshBuffer(self.handle, other.handle, transformation.handle)
    else
        NATIVE.AppendMeshBuffer(self.handle, other.handle)
    end
end

function MeshBuffer:appendIndex( index )
    NATIVE.AppendIndexToMeshBuffer(self.handle, index)
end

function MeshBuffer:appendVertex( vertex )
    local v = vertex
    NATIVE.AppendVertexToMeshBuffer(
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
