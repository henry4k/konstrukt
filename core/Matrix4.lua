local class  = require 'middleclass'
local Vec    = require 'core/Vector'
local CreateMatrix4          = ENGINE.CreateMatrix4
local CopyMatrix4            = ENGINE.CopyMatrix4
local Matrix4Op              = ENGINE.Matrix4Op
local TranslateMatrix4       = ENGINE.TranslateMatrix4
local ScaleMatrix4           = ENGINE.ScaleMatrix4
local RotateMatrix4          = ENGINE.RotateMatrix4
local Matrix4TransformVector = ENGINE.Matrix4TransformVector
local MakeRotationMatrix     = ENGINE.MakeRotationMatrix


local Matrix4 = class('core/Matrix4')

function Matrix4:initialize( handle )
    self.handle = handle or CreateMatrix4()
end

function Matrix4:copy()
    return Matrix4(CopyMatrix4(self.handle))
end

function Matrix4:__add( other )
    return Matrix4(Matrix4Op(self.handle, other.handle, '+'))
end

function Matrix4:__sub( other )
    return Matrix4(Matrix4Op(self.handle, other.handle, '-'))
end

function Matrix4:__mul( other )
    return Matrix4(Matrix4Op(self.handle, other.handle, '*'))
end

function Matrix4:__div( other )
    return Matrix4(Matrix4Op(self.handle, other.handle, '/'))
end

function Matrix4:translate( vec )
    return Matrix4(TranslateMatrix4(self.handle,
                                    vec[1],
                                    vec[2],
                                    vec[3]))
end

function Matrix4:scale( v )
    if type(v) == 'number' then
        v = Vec(v,v,v)
    end
    return Matrix4(ScaleMatrix4(self.handle,
                                v[1],
                                v[2],
                                v[3]))
end

function Matrix4:rotate( angle, vec )
    return Matrix4(RotateMatrix4(self.handle,
                                 angle,
                                 vec[1],
                                 vec[2],
                                 vec[3]))
end

function Matrix4:transform( vec )
    return Vec(Matrix4TransformVector(self.handle,
                                      vec[1],
                                      vec[2],
                                      vec[3],
                                      vec[4]))
end

function Matrix4:toRotationMatrix()
    return Matrix4(MakeRotationMatrix(self.handle))
end


return Matrix4
