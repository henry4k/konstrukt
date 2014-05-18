local m = {}
m.__index = m

function m.newFromHandle( handle )
    local self = setmetatable({}, m)
    self.handle = handle
    return self
end

function m.new()
    return m.newFromHandle(NATIVE.CreateMeshBuffer())
end

function m:transform( self, transformation )
    NATIVE.TransformMeshBuffer(self.handle, transformation.handle)
end

function m:appendMeshBuffer( self, other, transformation )
    if transformation then
        NATIVE.AppendMeshBuffer(self.handle, other.handle, transformation.handle)
    else
        NATIVE.AppendMeshBuffer(self.handle, other.handle)
    end
end

function m:appendIndex( self, index )
    NATIVE.AppendIndexToMeshBuffer(self.handle, index)
end

function m:appendVertex( self, vertex )

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

return m
