local class = require 'core/middleclass'
local Vec   = require 'core/Vector'
local Mat4  = require 'core/Matrix4'


local ShaderProgram = class('core/ShaderProgram')

--- Links the given `shaders` into a shader program.
function ShaderProgram:initialize( ... )
    local shaders = {...}
    local shaderHandles = {}
    for i,v in ipairs(shaders) do
        shaderHandles[i] = v.handle
    end
    self.handle = NATIVE.LinkShaderProgram(table.unpack(shaderHandles))
end

function ShaderProgram:setUniform( name, value )
    if class.Object.isInstanceOf(value, Mat4) then
        NATIVE.SetMatrix4Unifrom(self.handle, name, value.handle)
    elseif class.Object.isInstanceOf(value, Vec) then
        NATIVE.SetVectorUnifrom(self.handle, name, value:unpack())
    else
        NATIVE.SetFloatUnifrom(self.handle, name, value)
    end
end


return ShaderProgram
