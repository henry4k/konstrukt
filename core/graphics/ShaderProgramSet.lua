--- Some effects, like shadow mapping, require models to be rendered
--- with a separate set of shader programs, that is adapted to the effect.
--
-- Shader program sets group programs into families.
-- A model just needs to save the family name, which is then resolved by
-- the used shader program set.
-- E.g. there can be a family for static and one for animated models.
--
-- @module core.graphics.ShaderProgramSet
-- @see ShaderProgram


local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local ShaderProgram           = require 'core/graphics/ShaderProgram'
local CreateShaderProgramSet  = ENGINE.CreateShaderProgramSet
local DestroyShaderProgramSet = ENGINE.DestroyShaderProgramSet
local SetShaderProgramFamily  = ENGINE.SetShaderProgramFamily


local ShaderProgramSet = class('core/graphics/ShaderProgramSet')

function ShaderProgramSet:initialize( defaultShaderProgram )
    assert(Object.isInstanceOf(defaultShaderProgram, ShaderProgram),
           'Must be initialized with a default shader program.')
    self.handle = CreateShaderProgramSet(defaultShaderProgram.handle)
end

function ShaderProgramSet:destroy()
    DestroyShaderProgramSet(self.handle)
    self.handle = nil
end

function ShaderProgramSet:setFamily( name, shaderProgram )
    assert(Object.isInstanceOf(shaderProgram, ShaderProgram),
           'Must be called with a shader program.')
    SetShaderProgramFamily(self.handle, name, shaderProgram.handle)
end


return ShaderProgramSet
