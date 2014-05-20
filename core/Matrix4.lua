local m = {}
m.__index = m

function m.newFromHandle( handle )
    local self = setmetatable({}, m)
    self.handle = handle
    return self
end

function m.new()
    return m.newFromHandle(NATIVE.CreateMatrix4())
end

function m:copy()
    return m.newFromHandle(NATIVE.CopyMatrix4(self.handle))
end

function m:__add( other )
    return m.newFromHandle(NATIVE.AddMatrix4(self, other))
end

function m:__sub( other )
    return m.newFromHandle(NATIVE.SubMatrix4(self, other))
end

function m:__mul( other )
    return m.newFromHandle(NATIVE.MulMatrix4(self, other))
end

function m:__div( other )
    return m.newFromHandle(NATIVE.DivMatrix4(self, other))
end

function m:translate( x, y, z )
    return m.newFromHandle(NATIVE.TranslateMatrix4(self.handle, x, y, z))
end

function m:scale( x, y, z )
    return m.newFromHandle(NATIVE.ScaleMatrix4(self.handle, x, y, z))
end

function m:rotate( angle, x, y, z )
    return m.newFromHandle(NATIVE.RotateMatrix4(self.handle, angle, x, y, z))
end

function m:transform( x, y, z, w )
    w = w or 0
    return NATIVE.Matrix4TransformVector(self.handle, x, y, z, w)
end

function m:toRotationMatrix()
    return m.newFromHandle(NATIVE.MakeRotationMatrix(self.handle))
end

return m
