local class           = require 'middleclass'
local Vec             = require 'core/Vector'
local Mat4            = require 'core/Matrix4'
local Resource        = require 'core/Resource'
local ResourceManager = require 'core/ResourceManager'
local LinkShaderProgram    = ENGINE.LinkShaderProgram
local DestroyShaderProgram = ENGINE.DestroyShaderProgram
local SetGlobalUniform     = ENGINE.SetGlobalUniform
local UnsetGlobalUniform   = ENGINE.UnsetGlobalUniform


local ShaderProgram = class('core/ShaderProgram')
ShaderProgram:include(Resource)

function ShaderProgram.static:load( ... )
    local shaders = {...}
    for i,shader in ipairs(shaders) do
        if type(shader) == 'string' then
            shaders[i] = ResourceManager.load('core/Shader', shader)
        end
    end
    return ShaderProgram(table.unpack(shaders))
end

--- Links the given `shaders` into a shader program.
function ShaderProgram:initialize( ... )
    local shaders = {...}
    local shaderHandles = {}
    for i,v in ipairs(shaders) do
        shaderHandles[i] = v.handle
    end
    self.handle = LinkShaderProgram(table.unpack(shaderHandles))
end

function ShaderProgram:destroy()
    DestroyShaderProgram(self.handle)
    self.handle = nil
end

function ShaderProgram.static:setGlobalUniform( name, value, type )
    if class.Object.isInstanceOf(value, Mat4) then
        SetGlobalUniform(name, 'mat4', value.handle)
    elseif class.Object.isInstanceOf(value, Vec) then
        SetGlobalUniform(name, 'vec'..#value, value:unpack())
    else
        SetGlobalUniform(name, type, value)
    end
end

function ShaderProgram.static:unsetGlobalUniform( name )
    UnsetGlobalUniform(name)
end


return ShaderProgram
