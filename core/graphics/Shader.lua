--- @classmod core.graphics.Shader
--- Shaders are compiled shader sources, like objects are compiled `.c/.cpp` sources.
--
-- Shaders alone aren't of any use.  They first need to be linked to into a
-- @{core.graphics.ShaderProgram}, which can then be used for rendering by the
-- graphics driver.
--
-- This is just a brief summary, the complete documentation is available at
-- http://www.opengl.org/wiki/GLSL_Object
--
-- Includes @{core.Resource}.
--
-- @see core.graphics.ShaderProgram


local engine   = require 'engine'
local class    = require 'middleclass'
local Resource = require 'core/Resource'


local Shader = class('core/graphics/Shader')
Shader:include(Resource)

--- Creates a shader the given shader source.
--
-- The shader type is determined by the file extension automatically.
--
-- @function static:load( fileName )
--
function Shader.static:_load( fileName )
    local shader = Shader(fileName)
    return { value=shader, destructor=shader.destroy }
end

function Shader:initialize( fileName )
    self.handle = engine.LoadShader(fileName)
end

function Shader:destroy()
    engine.DestroyShader(self.handle)
    self.handle = nil
end


return Shader
