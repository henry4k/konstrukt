local class = require 'middleclass'


local Matrix4 = class('core/Matrix4')

function Matrix4:initialize( handle )
    self.handle = handle or NATIVE.CreateMatrix4()
end

function Matrix4:copy()
    return Matrix4:new(NATIVE.CopyMatrix4(self.handle))
end

function Matrix4:__add( other )
    return Matrix4:new(NATIVE.AddMatrix4(self, other))
end

function Matrix4:__sub( other )
    return Matrix4:new(NATIVE.SubMatrix4(self, other))
end

function Matrix4:__mul( other )
    return Matrix4:new(NATIVE.MulMatrix4(self, other))
end

function Matrix4:__div( other )
    return Matrix4:new(NATIVE.DivMatrix4(self, other))
end

function Matrix4:translate( x, y, z )
    return Matrix4:new(NATIVE.TranslateMatrix4(self.handle, x, y, z))
end

function Matrix4:scale( x, y, z )
    return Matrix4:new(NATIVE.ScaleMatrix4(self.handle, x, y, z))
end

function Matrix4:rotate( angle, x, y, z )
    return Matrix4:new(NATIVE.RotateMatrix4(self.handle, angle, x, y, z))
end

function Matrix4:transform( x, y, z, w )
    w = w or 0
    return NATIVE.Matrix4TransformVector(self.handle, x, y, z, w)
end

function Matrix4:toRotationMatrix()
    return Matrix4:new(NATIVE.MakeRotationMatrix(self.handle))
end


return Matrix4
