local m = {}
m.__index = m

function m.newFromHandle( handle )
    local self = setmetatable({}, m)
    self.handle = handle
    return self
end

function m.new( meshBuffer )
    return m.newFromHandle(NATIVE.CreateMesh(meshBuffer))
end

return m
