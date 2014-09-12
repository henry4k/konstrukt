local class = require 'core/middleclass'



--- Some effects, like shadow mapping, require models to be rendered
--- with a separate set of shader programs, that is adapted to the effect.
--
-- Shader program sets group programs into families.
-- A model just needs to save the family name, which is then resolved by
-- the used shader program set.
-- E.g. there can be a family for static and one for animated models.
local ShaderProgramSet = class('core/ShaderProgramSet')

function ShaderProgramSet:initialize( defaultShaderProgram )
    self.handle = NATIVE.CreateShaderProgramSet(defaultShaderProgram.handle)
end

function ShaderProgramSet:destroy()
    NATIVE.DestroyShaderProgramSet(self.handle)
    self.handle = nil
end

function ShaderProgramSet:setFamily( name, shaderProgram )
    NATIVE.SetShaderProgramFamily(self.handle, name, shaderProgram.handle)
end


return ShaderProgramSet
