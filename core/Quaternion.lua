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

function Quat:initialize( ... )
    local argCount = select('#', ...)
    if argCount == 0 then
        self.handle = handle or CreateQuaternion()
    elseif argCount == 1 then
        local arg = ...
        if Object.isInstanceOf(arg, Quat) then
            self.handle = CopyQuaternion(arg.handle)
        elseif Object.isInstanceOf(arg, Vec) then
            self.handle = CreateQuaternionFromEulerAngles(arg:unpack(3))
        elseif Object.isInstanceOf(arg, Mat4) then
            self.handle = CreateQuaternionFromMatrix(arg.handle)
        end
    elseif argCount == 3 then
        self.handle = CreateQuaternionFromEulerAngles(...)
    else
        error('Unknown constructor syntax.')
    end
end

function Quat:copy()
    return Quat(CopyQuaternion(self.handle))
end

function Quat:__add( other )
    return Quat(QuaternionOp(self.handle, other.handle, '+'))
end

function Quat:__mul( other )
    return Quat(QuaternionOp(self.handle, other.handle, '*'))
end

function Quat:lerp( factor, other )
    return Quat(LerpQuaternion(self.handle, other.handle, 'l'))
end

function Quat:slerp( factor, other )
    return Quat(LerpQuaternion(self.handle, other.handle, 's'))
end

function Quat:__add( other )
    return Quat(QuaternionOp(self.handle, other.handle, '+'))
end

function Quat:__mul( other )
    return Quat(QuaternionOp(self.handle, other.handle, '*'))
end


return Quat
