--- Shader programs are used to define how models are rendered.
--
-- They consist of multiple @{Shader} objects, which must provide an entry point
-- for each mandatory rendering stage:  Vertex and fragment shader.
--
-- This is just a brief summary, the complete documentation is available at
-- http://www.opengl.org/wiki/GLSL_Object
--
-- Includes @{Resource}.
--
-- @classmod core.graphics.ShaderProgram


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

--- Creates a shader program by loading and linking the given shader sources.
--
-- @function static:load( ... )
--
-- @param[type=string] ...
-- Multiple shader sources that are then loaded, compiled and linked into the
-- program.
--
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

--- Links the given @{Shader}s into a shader program.
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

--- Sets a global default value for a uniform.
--
-- @param name
-- @param[type=number|Vector|Matrix4] value
-- @param[opt] type
-- Is needed for number unifroms.  Either `integer` or `float` are applicable.
--
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

--- Remove a global uniform.
--
-- Note that the uniform may still be set in some shader programs.
--
function ShaderProgram.static:unsetGlobalUniform( name )
    UnsetGlobalUniform(name)
end


return ShaderProgram
