local class  = require 'middleclass'
local Object = class.Object
local Mat4   = require 'apoapsis.core.Matrix4'
local engine = require 'apoapsis.engine'
local CreateQuaternion                = engine.CreateQuaternion
local CopyQuaternion                  = engine.CopyQuaternion
local CreateQuaternionFromEulerAngles = engine.CreateQuaternionFromEulerAngles
local CreateQuaternionFromMatrix      = engine.CreateQuaternionFromMatrix
local QuaternionOp                    = engine.QuaternionOp
local LerpQuaternion                  = engine.LerpQuaternion
local LerpQuaternion                  = engine.LerpQuaternion


local Quat = class('apoapsis/core/Quaternion')

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
