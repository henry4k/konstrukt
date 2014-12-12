---
-- @module core.graphics.ShaderProgram


local assert          = assert
local class           = require 'middleclass'
local Object          = class.Object
local Vec             = require 'core/Vector'
local Mat4            = require 'core/Matrix4'
local Shader          = require 'core/graphics/Shader'
local Resource        = require 'core/Resource'
local LinkShaderProgram    = ENGINE.LinkShaderProgram
local DestroyShaderProgram = ENGINE.DestroyShaderProgram
local SetGlobalUniform     = ENGINE.SetGlobalUniform
local UnsetGlobalUniform   = ENGINE.UnsetGlobalUniform


local ShaderProgram = class('core/graphics/ShaderProgram')
ShaderProgram:include(Resource)

---
-- @local
-- @param ...
function ShaderProgram.static:_load( ... )
    local shaders = {...}
    for i,shader in ipairs(shaders) do
        if type(shader) == 'string' then
            shaders[i] = Shader:load(shader)
        else
            assert(Object.isInstanceOf(shader, Shader))
        end
    end
    local shaderProgram = ShaderProgram(table.unpack(shaders))
    return { value=shaderProgram, destructor=shaderProgram.destroy }
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

---
function ShaderProgram:destroy()
    DestroyShaderProgram(self.handle)
    self.handle = nil
end

---
function ShaderProgram.static:setGlobalUniform( name, value, type )
    if class.Object.isInstanceOf(value, Mat4) then
        assert(not type, 'Type argument is ignored, when called with a matrix.')
        SetGlobalUniform(name, 'mat4', value.handle)
    elseif class.Object.isInstanceOf(value, Vec) then
        assert(not type, 'Type argument is ignored, when called with a vector.')
        SetGlobalUniform(name, 'vec'..#value, value:unpack())
    else
        assert(type, 'Type is missing.')
        SetGlobalUniform(name, type, value)
    end
end

---
function ShaderProgram.static:unsetGlobalUniform( name )
    UnsetGlobalUniform(name)
end


return ShaderProgram
