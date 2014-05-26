local class = require 'middleclass'
local Matrix4 = require 'core/Matrix4'


local ShaderProgram = class('core/ShaderProgram')

function ShaderProgram:initialize( ... )
    local shaders = {...}
    local shaderHandles = {}
    for i,v in ipairs(shaders) do
        shaderHandles[i] = v.handle
    end
    self.handle = NATIVE.LinkShaderProgram(table.unpack(shaderHandles))
end

function ShaderProgram:setNumberUniform( name, number )
    NATIVE.SetFloatUnifrom(self.handle, name, number)
end

function ShaderProgram:setVectorUniform( name, ... )
    NATIVE.SetVectorUnifrom(self.handle, name, ...)
end

function ShaderProgram:setMatrixUniform( name, matrix )
    NATIVE.SetMatrix4Unifrom(self.handle, name, matrix.handle)
end

function ShaderProgram:setUniform( name, ... )
    local argCount = select('#', ...)
    if argCount == 1 then
        local arg = ...
        if class.Object.isInstanceOf(arg, Matrix4) then
            self:setMatrix4Uniform(name, arg)
        else
            self:setNumberUniform(name, arg)
        end
    else
        self:setVectorUniform(name, ...)
    end
end


return ShaderProgram
