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


local engine = require 'engine'
local class  = require 'middleclass'
local Object = class.Object
local Vec    = require 'core/Vector'
local Mat4   = require 'core/Matrix4'


local Quat = class('core/Quaternion')

--- Create a new quaternion.
--
-- @usage
-- Quat() -- New identity quaternion.
-- Quat( quat ) -- Copy quaternion.
function Quat:initialize( quat )
    quat = quat or engine.CreateQuaternion()

    if type(quat) == 'userdata' then
        self.handle = quat
    elseif Object.isInstanceOf(quat, Quat) then
        self.handle = engine.CopyQuaternion(quat.handle)
    else
        error('Invalid constructor use.')
    end
end

function Quat.static:byAngleAndAxis( w, x, y, z )
    if Vec:isInstance(x) then
        return Quat(engine.CreateQuaternionByAngleAndAxis(w, x:unpack(3)))
    else
        return Quat(engine.CreateQuaternionByAngleAndAxis(w, x, y, z))
    end
end

function Quat.static:fromEulerAngles( x, y, z )
    if Vec:isInstance(x) then
        return Quat(engine.CreateQuaternionFromEulerAngles(x:unpack(3)))
    else
        return Quat(engine.CreateQuaternionFromEulerAngles(x, y, z))
    end
end

function Quat.static:fromMatrix( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Argument must be a matrix.')
    return Quat(engine.CreateQuaternionFromMatrix(matrix.handle))
end

--- Create an independent copy of the instance.
function Quat:clone()
    return Quat(engine.CopyQuaternion(self.handle))
end

--- Create a roation matrix from this quaternion.
function Quat:toMatrix()
    return Mat4(engine.CreateMatrix4FromQuaternion(self.handle))
end

--- Computes the unit quaternion.
function Quat:normalize()
    return Quat(engine.NormalizeQuaternion(self.handle))
end

--- Get the quaternion conjugate.
function Quat:conjugate()
    return Quat(engine.QuaternionConjugate(self.handle))
end

--- Invert the orientation.
function Quat:__unm()
    return Quat(engine.InvertQuaternion(self.handle))
end

--- Add another quaternion.
function Quat:__add( other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(engine.QuaternionOp(self.handle, other.handle, '+'))
end

--- Multiply with another quaternion.
function Quat:__mul( other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with a quaternion.')
    return Quat(engine.QuaternionOp(self.handle, other.handle, '*'))
end

--- Multiply with a vector.
--
-- One parameter must be an vector and one a quaternion.
--
function Quat.static:multiplyVector( a, b )
    if Object.isInstanceOf(a, Quat) and Vec:isInstance(b) then
        return Vec(engine.QuaternionXVector3(a.handle,
                                             b[1],
                                             b[2],
                                             b[3]))
    elseif Vec:isInstance(a) and Object.isInstanceOf(b, Quat) then
        return Vec(engine.Vector3XQuaternion(a[1],
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
    return Quat(engine.LerpQuaternion(self.handle, other.handle, 'l', factor))
end

--- Spherical linear interpolation with another quaternion.
function Quat:slerp( factor, other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(engine.LerpQuaternion(self.handle, other.handle, 's', factor))
end


return Quat
