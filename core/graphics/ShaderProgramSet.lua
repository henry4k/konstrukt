--- @classmod core.graphics.ShaderProgramSet
--- ShaderProgramSets group programs into families.
--
-- A model just needs to save the family name, which is then resolved by
-- the used shader program set.
-- E.g. there can be a family for static and one for animated models.
--
-- Some effects, like shadow mapping, require models to be rendered
-- with a special shader programs, that are adapted to the effect.
--
-- @see core.graphics.ShaderProgram


local engine = require 'engine'
local class  = require 'middleclass'
local Object = class.Object
local Scheduler = require 'core/Scheduler'
local ShaderProgram = require 'core/graphics/ShaderProgram'


local ShaderProgramSet = class('core/graphics/ShaderProgramSet')

---
-- @param[type=core.graphics.ShaderProgram] defaultShaderProgram
-- Shader program which is used as a fallback, if a the required family is not
-- available in this set.
--
function ShaderProgramSet:initialize( defaultShaderProgram )
    assert(Object.isInstanceOf(defaultShaderProgram, ShaderProgram),
           'Must be initialized with a default shader program.')
    self.handle = Scheduler.awaitCall(engine.CreateShaderProgramSet, defaultShaderProgram.handle)
end

function ShaderProgramSet:destroy()
    Scheduler.blindCall(engine.DestroyShaderProgramSet, self.handle)
    self.handle = nil
end

--- Register a shader program for a specific family.
function ShaderProgramSet:setFamily( name, shaderProgram )
    assert(Object.isInstanceOf(shaderProgram, ShaderProgram),
           'Must be called with a shader program.')
    Scheduler.blindCall(engine.SetShaderProgramFamily, self.handle, name, shaderProgram.handle)
end


return ShaderProgramSet
