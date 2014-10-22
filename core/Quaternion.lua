local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Mat4   = require 'core/Matrix4'
local CreateQuaternion                = ENGINE.CreateQuaternion
local CopyQuaternion                  = ENGINE.CopyQuaternion
local CreateQuaternionFromEulerAngles = ENGINE.CreateQuaternionFromEulerAngles
local CreateQuaternionFromMatrix      = ENGINE.CreateQuaternionFromMatrix
local QuaternionOp                    = ENGINE.QuaternionOp
local LerpQuaternion                  = ENGINE.LerpQuaternion
local LerpQuaternion                  = ENGINE.LerpQuaternion


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
    elseif Object.isInstanceOf(arg, Vec) then
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

function Quat:__add( other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(QuaternionOp(self.handle, other.handle, '+'))
end

function Quat:__mul( other )
    assert(Object.isInstanceOf(other, Quat), 'Must be called with an quaternion.')
    return Quat(QuaternionOp(self.handle, other.handle, '*'))
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
