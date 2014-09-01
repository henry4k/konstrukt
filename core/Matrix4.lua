local class = require 'core/middleclass'
local Vec   = require 'core/Vector'


local Matrix4 = class('core/Matrix4')

function Matrix4:initialize( handle )
    self.handle = handle or NATIVE.CreateMatrix4()
end

function Matrix4:copy()
    return Matrix4(NATIVE.CopyMatrix4(self.handle))
end

function Matrix4:__add( other )
    return Matrix4(NATIVE.Matrix4Op(self.handle, other.handle, '+'))
end

function Matrix4:__sub( other )
    return Matrix4(NATIVE.Matrix4Op(self.handle, other.handle, '-'))
end

function Matrix4:__mul( other )
    return Matrix4(NATIVE.Matrix4Op(self.handle, other.handle, '*'))
end

function Matrix4:__div( other )
    return Matrix4(NATIVE.Matrix4Op(self.handle, other.handle, '/'))
end

function Matrix4:translate( vec )
    return Matrix4(NATIVE.TranslateMatrix4(self.handle,
                                           vec[1],
                                           vec[2],
                                           vec[3]))
end

function Matrix4:scale( vec )
    return Matrix4(NATIVE.ScaleMatrix4(self.handle,
                                       vec[1],
                                       vec[2],
                                       vec[3]))
end

function Matrix4:rotate( angle, vec )
    return Matrix4(NATIVE.RotateMatrix4(self.handle,
                                        angle,
                                        vec[1],
                                        vec[2],
                                        vec[3]))
end

function Matrix4:transform( vec )
    return Vec(NATIVE.Matrix4TransformVector(self.handle,
                                             vec[1],
                                             vec[2],
                                             vec[3],
                                             vec[4]))
end

function Matrix4:toRotationMatrix()
    return Matrix4(NATIVE.MakeRotationMatrix(self.handle))
end


return Matrix4
