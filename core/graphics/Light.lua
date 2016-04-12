--- @classmod core.graphics.Light
--- A light source.

local engine  = require 'engine'
local class   = require 'middleclass'
local ShaderVariableSet   = require 'core/graphics/ShaderVariableSet'
local HasTransformation   = require 'core/HasTransformation'
local HasAttachmentTarget = require 'core/physics/HasAttachmentTarget'


local Light = class('core/graphics/Light')
Light:include(HasTransformation)
Light:include(HasAttachmentTarget)

--- Can't be instanciated directly, use @{core.graphics.LightWorld:createLight} instead.
function Light:initialize()
    assert(self.handle, 'Can\'t be instanciated directly, use LightWorld:createLight instead.')
    self.shaderVariables =
        ShaderVariableSet(engine.GetLightShaderVariableSet(self.handle))
    self.attachmentTarget = nil
end

function Light:destroy()
    engine.DestroyLight(self.handle)
    self.handle = nil
    self.shaderVariables:destroy()
    self.shaderVariables = nil
end

function Light:setValue( value )
    assert(value >= 0, 'Value must be positive or zero.')
    engine.SetLightValue(self.handle, value)
end

function Light:setRange( range )
    assert(range >= 0, 'Range must be positive or zero.')
    engine.SetLightRange(self.handle, range)
end

function Light:_setTransformation( matrix )
    engine.SetLightTransformation(self.handle, matrix.handle)
end

function Light:_setAttachmentTarget( solidHandle, flags )
    engine.SetLightAttachmentTarget(self.handle, solidHandle, flags)
end


return Light
