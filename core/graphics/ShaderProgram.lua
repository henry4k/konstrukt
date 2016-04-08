--- @classmod core.graphics.ShaderProgram
--- Shader programs are used to define how models are rendered.
--
-- They consist of multiple @{core.graphics.Shader} objects, which must provide
-- an entry point for each mandatory rendering stage: Vertex and fragment shader.
--
-- This is just a brief summary, the complete documentation is available at
-- http://www.opengl.org/wiki/GLSL_Object
--
-- Includes @{core.Resource}.


local engine   = require 'engine'
local class    = require 'middleclass'
local Object   = class.Object
local Vec      = require 'core/Vector'
local Mat4     = require 'core/Matrix4'
local Resource = require 'core/Resource'
local Shader   = require 'core/graphics/Shader'
local ShaderVariableSet = require 'core/graphics/ShaderVariableSet'


local ShaderProgram = class('core/graphics/ShaderProgram')
ShaderProgram:include(Resource)

ShaderProgram.static.globalVariables =
    ShaderVariableSet(engine.GetGlobalShaderVariableSet())

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

--- Links the given @{core.graphics.Shader}s into a shader program.
function ShaderProgram:initialize( ... )
    local shaders = {...}
    local shaderHandles = {}
    for i,v in ipairs(shaders) do
        shaderHandles[i] = v.handle
    end
    self.handle = engine.LinkShaderProgram(table.unpack(shaderHandles))
    self.variables =
        ShaderVariableSet(engine.GetShaderProgramShaderVariableSet(self.handle))
end

function ShaderProgram:destroy()
    engine.DestroyShaderProgram(self.handle)
    self.handle = nil
    self.variables:destroy()
    self.variables = nil
end

--- Sets a global default value for a uniform.
--
-- @param name
-- @param[type=number|core.Vector|core.Matrix4] value
-- @param[opt] type
-- Is needed for number unifroms.  Either `int` or `float` are applicable.
--
function ShaderProgram.static:setGlobalUniform( name, value, type )
    if class.Object.isInstanceOf(value, Mat4) then
        assert(not type, 'Type argument is ignored, when called with a matrix.')
        engine.SetGlobalUniform(name, 'mat4', value.handle)
    elseif class.Object.isInstanceOf(value, Vec) then
        assert(not type, 'Type argument is ignored, when called with a vector.')
        engine.SetGlobalUniform(name, 'vec'..#value, value:unpack())
    else
        assert(type, 'Type is missing.')
        engine.SetGlobalUniform(name, type, value)
    end
end

--- Remove a global uniform.
--
-- Note that the uniform may still be set in some shader programs.
--
function ShaderProgram.static:unsetGlobalUniform( name )
    engine.UnsetGlobalUniform(name)
end


return ShaderProgram
