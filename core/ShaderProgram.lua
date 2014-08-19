local class = require 'core/middleclass'
local Vec   = require 'core/Vector'
local Mat4  = require 'core/Matrix4'


local ShaderProgram = class('core/ShaderProgram')

function ShaderProgram:initialize( ... )
    local shaders = {...}
    local shaderHandles = {}
    for i,v in ipairs(shaders) do
        shaderHandles[i] = v.handle
    end
    self.handle = NATIVE.LinkShaderProgram(table.unpack(shaderHandles))
end

function ShaderProgram:setUniform( name, ... )
    local argCount = select('#', ...)
    if argCount == 1 then
        local arg = ...
        if class.Object.isInstanceOf(arg, Mat4) then
            self:setMatrix4Uniform(name, arg)
        elseif class.Object.isInstanceOf(arg, Vec) then
            self:_setVectorUniform(name, table.unpack(arg))
        else
            self:_setNumberUniform(name, arg)
        end
    else
        self:_setVectorUniform(name, ...)
    end
end

function ShaderProgram:_setNumberUniform( name, number )
    NATIVE.SetFloatUnifrom(self.handle, name, number)
end

function ShaderProgram:_setVectorUniform( name, ... )
    NATIVE.SetVectorUnifrom(self.handle, name, ...)
end

function ShaderProgram:_setMatrixUniform( name, matrix )
    NATIVE.SetMatrix4Unifrom(self.handle, name, matrix.handle)
end


return ShaderProgram
