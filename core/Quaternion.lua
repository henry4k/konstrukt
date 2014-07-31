local class  = require 'core/middleclass.lua'
local Object = class.Object
local Mat4   = require 'core/Matrix4.lua'


local Quat = class('core/Quaternion')

function Quat:initialize( ... )
    local argCount = select('#', ...)
    if argCount == 0 then
        self.handle = handle or NATIVE.CreateQuaternion()
    elseif argCount == 1 then
        local arg = ...
        if Object.isInstanceOf(arg, Quat) then
            self.handle = NATIVE.CopyQuaternion(arg.handle)
        elseif Object.isInstanceOf(arg, Vec) then
            self.handle = NATIVE.CreateQuaternionFromEulerAngles(arg:unpack(3))
        elseif Object.isInstanceOf(arg, Mat4) then
            self.handle = NATIVE.CreateQuaternionFromMatrix(arg.handle)
        end
    elseif argCount == 3 then
        self.handle = NATIVE.CreateQuaternionFromEulerAngles(...)
    else
        error('Unknown constructor syntax.')
    end
end

function Quat:copy()
    return Quat:new(NATIVE.CopyQuaternion(self.handle))
end

function Quat:__add( other )
    return Quat:new(NATIVE.QuaternionOp(self.handle, other.handle, '+'))
end

function Quat:__mul( other )
    return Quat:new(NATIVE.QuaternionOp(self.handle, other.handle, '*'))
end

function Quat:lerp( factor, other )
    return Quat:new(NATIVE.LerpQuaternion(self.handle, other.handle, 'l'))
end

function Quat:slerp( factor, other )
    return Quat:new(NATIVE.LerpQuaternion(self.handle, other.handle, 's'))
end

function Quat:__add( other )
    return Quat:new(NATIVE.QuaternionOp(self.handle, other.handle, '+'))
end

function Quat:__mul( other )
    return Quat:new(NATIVE.QuaternionOp(self.handle, other.handle, '*'))
end


return Quat
