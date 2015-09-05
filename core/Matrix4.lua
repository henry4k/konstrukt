--- @classmod core.Matrix4
--- A 4x4 matrix class.

local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Vec    = require 'core/Vector'
local CreateMatrix4          = ENGINE.CreateMatrix4
local CopyMatrix4            = ENGINE.CopyMatrix4
local Matrix4Op              = ENGINE.Matrix4Op
local TranslateMatrix4       = ENGINE.TranslateMatrix4
local ScaleMatrix4           = ENGINE.ScaleMatrix4
local RotateMatrix4          = ENGINE.RotateMatrix4
local Matrix4TransformVector = ENGINE.Matrix4TransformVector
local CreateLookAtMatrix     = ENGINE.CreateLookAtMatrix
local MakeRotationMatrix     = ENGINE.MakeRotationMatrix


local Matrix4 = class('core/Matrix4')

function Matrix4:initialize( handle )
    assert(not handle or type(handle) == 'userdata')
    self.handle = handle or CreateMatrix4()
end

--- Create an independent copy of the instance.
function Matrix4:clone()
    return Matrix4(CopyMatrix4(self.handle))
end

--- Create a look-at matrix.
--
-- @param[type=core.Vector] eye
-- @param[type=core.Vector] center
-- @param[type=core.Vector] up
function Matrix4.static:lookAt( eye, center, up )
    return self(CreateLookAtMatrix(eye[1],
                                   eye[2],
                                   eye[3],
                                   center[1],
                                   center[2],
                                   center[3],
                                   up[1],
                                   up[2],
                                   up[3]))
end

--- Add another matrix.
function Matrix4:__add( other )
    assert(Object.isInstanceOf(other, Matrix4), 'Must be called with a matrix.')
    return Matrix4(Matrix4Op(self.handle, other.handle, '+'))
end

--- Substract another matrix.
function Matrix4:__sub( other )
    assert(Object.isInstanceOf(other, Matrix4), 'Must be called with a matrix.')
    return Matrix4(Matrix4Op(self.handle, other.handle, '-'))
end

--- Multiply with another matrix.
function Matrix4:__mul( other )
    assert(Object.isInstanceOf(other, Matrix4), 'Must be called with a matrix.')
    return Matrix4(Matrix4Op(self.handle, other.handle, '*'))
end

--- Divide by another matrix.
function Matrix4:__div( other )
    assert(Object.isInstanceOf(other, Matrix4), 'Must be called with a matrix.')
    return Matrix4(Matrix4Op(self.handle, other.handle, '/'))
end

--- Translate along the given vector.
-- @param[type=core.Vector] v
function Matrix4:translate( v )
    assert(Vec:isInstance(v), 'Must be called with a vector.')
    return Matrix4(TranslateMatrix4(self.handle,
                                    v[1],
                                    v[2],
                                    v[3]))
end

--- Scale by the given vector.
-- @param[type=core.Vector] v
function Matrix4:scale( v )
    assert(Vec:isInstance(v) or type(v) == 'number',
           'Must be called with a vector or a number.')
    if type(v) == 'number' then
        v = Vec(v,v,v)
    end
    return Matrix4(ScaleMatrix4(self.handle,
                                v[1],
                                v[2],
                                v[3]))
end

--- Rotate around the given vector by `angle` radians.
-- @param angle
-- @param[type=core.Vector] vec
function Matrix4:rotate( angle, vec )
    assert(Vec:isInstance(vec), 'Must be called with a vector.')
    return Matrix4(RotateMatrix4(self.handle,
                                 angle,
                                 vec[1],
                                 vec[2],
                                 vec[3]))
end

--- Transform the given vector.
-- @return[type=core.Vector] The transformed vector.
function Matrix4:transform( vec )
    assert(Vec:isInstance(vec), 'Must be called with a vector.')
    return Vec(Matrix4TransformVector(self.handle,
                                      vec[1],
                                      vec[2],
                                      vec[3],
                                      vec[4]))
end

--- Creates a rotation matrix.
-- This simply strips the translation and skew components.
-- @return[type=core.Matrix4]
function Matrix4:toRotationMatrix()
    return Matrix4(MakeRotationMatrix(self.handle))
end


return Matrix4
