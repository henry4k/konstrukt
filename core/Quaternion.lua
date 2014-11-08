local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Vec    = require 'core/Vector'
local Mat4   = require 'core/Matrix4'
local CreateQuaternion                = ENGINE.CreateQuaternion
local CreateQuaternionFromEulerAngles = ENGINE.CreateQuaternionFromEulerAngles
local CreateQuaternionFromMatrix      = ENGINE.CreateQuaternionFromMatrix
local CopyQuaternion                  = ENGINE.CopyQuaternion
local NormalizeQuaternion             = ENGINE.NormalizeQuaternion
local InvertQuaternion                = ENGINE.InvertQuaternion
local QuaternionOp                    = ENGINE.QuaternionOp
local LerpQuaternion                  = ENGINE.LerpQuaternion
local QuaternionXVector3              = ENGINE.QuaternionXVector3
local Vector3XQuaternion              = ENGINE.Vector3XQuaternion
local CreateMatrix4FromQuaternion     = ENGINE.CreateMatrix4FromQuaternion


local Quat = class('core/Quaternion')

-- Quat() - New identity quaternion.
-- Quat( quat ) - Copy quaternion.
-- Quat( vec ), Quat( x, y, z ) - Create quaternion from euler angles.
-- Quat( mat ) - Create quaternion from matrix.
function Quat:initialize( arg )
    arg = arg or CreateQuaternion()

    if type(arg) == 'userdata' then
        self.handle = arg
    elseif Object.isInstanceOf(arg, Quat) then
        self.handle = CopyQuaternion(arg.handle)
    elseif Vec:isInstance(arg) then
        self.handle = CreateQuaternionFromEulerAngles(arg:unpack(3))
    elseif Object.isInstanceOf(arg, Mat4) then
        self.handle = CreateQuaternionFromMatrix(arg.handle)
    else
        error('Invalid constructor use.')
    end
end

function Quat:copy()
    return Quat(CopyQuaternion(self.handle))
end

function Quat:toMatrix()
    return Mat4(CreateMatrix4FromQuaternion(self.handle))
end

function Quat:normalize()
    return Quat(NormalizeQuaternion(self.handle))
end

function Quat:_unm()
    return Quat(InvertQuaternion(self.handle))
end

function Quat:__add( other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(QuaternionOp(self.handle, other.handle, '+'))
end

function Quat:__mul( other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with a quaternion.')
    return Quat(QuaternionOp(self.handle, other.handle, '*'))
end

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

function Quat:lerp( factor, other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(LerpQuaternion(self.handle, other.handle, 'l', factor))
end

function Quat:slerp( factor, other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(LerpQuaternion(self.handle, other.handle, 's', factor))
end


return Quat
