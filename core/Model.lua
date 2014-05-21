local m = {}
m.__index = m

function m.newFromHandle( handle )
    local self = setmetatable({}, m)
    self.handle = handle
    return self
end

function m.new()
    return m.newFromHandle(NATIVE.CreateModel())
end

function m:setMesh( mesh )
    NATIVE.SetModelMesh(self.handle, mesh.handle)
end

function m:setTransformation( transformation )
    NATIVE.SetModelTransformation(self.handle, transformation.handle)
end

function m:remove()
    NATIVE.RemoveModel(self.handle)
    self.handle = nil
end

return m
