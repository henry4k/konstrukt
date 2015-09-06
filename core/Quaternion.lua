--- @classmod core.Quaternion
--- @alias Quat
---
--
-- A set of imaginary numbers which describe a rotation in 3d space.
-- More information on them can be found e.g. on [wikipedia](http://en.wikipedia.org/wiki/Quaternion).
--
-- To cut it short, in contrast to euler rotation quaternions have ...
--
-- - ... less computational overhead.
-- - ... no [Gimbal Lock](http://en.wikipedia.org/wiki/Gimbal_lock).
-- - ... [better interpolation](https://www.youtube.com/watch?v=QxIdIZ0eKCE).


local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Vec    = require 'core/Vector'
local Mat4   = require 'core/Matrix4'
local CreateQuaternion                = ENGINE.CreateQuaternion
local CreateQuaternionByAngleAndAxis  = ENGINE.CreateQuaternionByAngleAndAxis
local CreateQuaternionFromEulerAngles = ENGINE.CreateQuaternionFromEulerAngles
local CreateQuaternionFromMatrix      = ENGINE.CreateQuaternionFromMatrix
local CopyQuaternion                  = ENGINE.CopyQuaternion
local NormalizeQuaternion             = ENGINE.NormalizeQuaternion
local QuaternionConjugate             = ENGINE.QuaternionConjugate
local InvertQuaternion                = ENGINE.InvertQuaternion
local QuaternionOp                    = ENGINE.QuaternionOp
local LerpQuaternion                  = ENGINE.LerpQuaternion
local QuaternionXVector3              = ENGINE.QuaternionXVector3
local Vector3XQuaternion              = ENGINE.Vector3XQuaternion
local CreateMatrix4FromQuaternion     = ENGINE.CreateMatrix4FromQuaternion


local Quat = class('core/Quaternion')

--- Create a new quaternion.
--
-- @usage
-- Quat() -- New identity quaternion.
-- Quat( quat ) -- Copy quaternion.
function Quat:initialize( quat )
    quat = quat or CreateQuaternion()

    if type(quat) == 'userdata' then
        self.handle = quat
    elseif Object.isInstanceOf(quat, Quat) then
        self.handle = CopyQuaternion(quat.handle)
    else
        error('Invalid constructor use.')
    end
end

function Quat.static:byAngleAndAxis( w, x, y, z )
    if Vec:isInstance(x) then
        return Quat(CreateQuaternionByAngleAndAxis(w, x:unpack(3)))
    else
        return Quat(CreateQuaternionByAngleAndAxis(w, x, y, z))
    end
end

function Quat.static:fromEulerAngles( x, y, z )
    if Vec:isInstance(x) then
        return Quat(CreateQuaternionFromEulerAngles(x:unpack(3)))
    else
        return Quat(CreateQuaternionFromEulerAngles(x, y, z))
    end
end

function Quat.static:fromMatrix( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Argument must be a matrix.')
    return Quat(CreateQuaternionFromMatrix(matrix.handle))
end

--- Create an independent copy of the instance.
function Quat:clone()
    return Quat(CopyQuaternion(self.handle))
end

--- Create a roation matrix from this quaternion.
function Quat:toMatrix()
    return Mat4(CreateMatrix4FromQuaternion(self.handle))
end

--- Computes the unit quaternion.
function Quat:normalize()
    return Quat(NormalizeQuaternion(self.handle))
end

--- Get the quaternion conjugate.
function Quat:conjugate()
    return Quat(QuaternionConjugate(self.handle))
end

--- Invert the orientation.
function Quat:__unm()
    return Quat(InvertQuaternion(self.handle))
end

--- Add another quaternion.
function Quat:__add( other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(QuaternionOp(self.handle, other.handle, '+'))
end

--- Multiply with another quaternion.
function Quat:__mul( other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with a quaternion.')
    return Quat(QuaternionOp(self.handle, other.handle, '*'))
end

--- Multiply with a vector.
--
-- One parameter must be an vector and one a quaternion.
--
function Quat.static:multiplyVector( a, b )
    if Object.isInstanceOf(a, Quat) and Vec:isInstance(b) then
        return Vec(QuaternionXVector3(a.handle,
                                      b[1],
                                      b[2],
                                      b[3]))
    elseif Vec:isInstance(a) and Object.isInstanceOf(b, Quat) then
        return Vec(Vector3XQuaternion(a[1],
                                      a[2],
                                      a[3],
                                      b.handle))
    else
        error('One parameter must be an vector and one a quaternion.')
    end
end

--- Linear interpolation with another quaternion.
function Quat:lerp( factor, other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(LerpQuaternion(self.handle, other.handle, 'l', factor))
end

--- Spherical linear interpolation with another quaternion.
function Quat:slerp( factor, other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(LerpQuaternion(self.handle, other.handle, 's', factor))
end


return Quat
