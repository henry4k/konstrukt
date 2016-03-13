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
local Shader   = require 'core/graphics/Shader'
local Resource = require 'core/Resource'


local ShaderProgram = class('core/graphics/ShaderProgram')
ShaderProgram:include(Resource)

--- Creates a shader program by linking the given shaders.
--
-- @function static:load( ... )
--
-- @param[type=Shader] ...
-- Multiple shaders which will be linked into the program.
--
function ShaderProgram.static:_load( ... )
    local shaders = {}

    for _, arg in ipairs({...}) do
        if Object.isInstanceOf(arg, Shader) then
            table.insert(shaders, arg)
        else
            for _, shader in ipairs(arg) do
                table.insert(shaders, shader)
            end
        end
    end

    for _, shader in ipairs(shaders) do
        assert(Object.isInstanceOf(shader, Shader))
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
end

function ShaderProgram:destroy()
    engine.DestroyShaderProgram(self.handle)
    self.handle = nil
end

--- Sets a global default value for a uniform.
--
-- @param name
-- @param[type=number|core.Vector|core.Matrix4] value
-- @param[opt] type
-- Is needed for number unifroms.  Either `integer` or `float` are applicable.
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
