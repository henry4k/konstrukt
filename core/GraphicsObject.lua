local m = {}
m.__index = m

function m.newFromHandle( handle )
    local self = setmetatable({}, m)
    self.handle = handle
    return self
end

function m.new()
    return m.newFromHandle(NATIVE.CreateGraphicsObject())
end

function m:setMesh( mesh )
    NATIVE.SetGraphicsObjectMesh(self.handle, mesh.handle)
end

function m:setTransformation( transformation )
    NATIVE.SetGraphicsObjectTransformation(self.handle, transformation.handle)
end

function m:remove()
    NATIVE.RemoveGraphicsObject(self.handle)
    self.handle = nil
end

return m
