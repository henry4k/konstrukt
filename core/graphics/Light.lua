--- @classmod core.graphics.Light
--- A light source.

local engine  = require 'engine'
local class   = require 'middleclass'
local Object  = class.Object
local Vec     = require 'core/Vector'
local Mat4    = require 'core/Matrix4'
local HasTransformation   = require 'core/HasTransformation'
local HasAttachmentTarget = require 'core/physics/HasAttachmentTarget'


local Light = class('core/graphics/Light')
Light:include(HasTransformation)
Light:include(HasAttachmentTarget)

--- Can't be instanciated directly, use @{core.graphics.LightWorld:createLight} instead.
function Light:initialize()
    assert(self.handle, 'Can\'t be instanciated directly, use LightWorld:createLight instead.')
    self.attachmentTarget = nil
    self.activeUniforms = {}
end

function Light:destroy()
    engine.DestroyLight(self.handle)
    self.handle = nil
end

function Light:_setValue( value )
    assert(value >= 0, 'Value must be positive or zero.')
    engine.SetLightValue(self.handle, value)
end

function Light:_setRange( range )
    assert(range >= 0, 'Range must be positive or zero.')
    engine.SetLightRange(self.handle, range)
end

--- Changes a light specific uniform.
--
-- @param name
-- @param[type=number|Vector|Matrix4] value
-- @param[opt] type
-- Is needed for number unifroms.  Either `int` or `float` are applicable.
--
function Light:_setUniform( name, value, type )
    if Object.isInstanceOf(value, Mat4) then
        assert(not type, 'Type argument is ignored, when called with a matrix.')
        engine.SetLightUniform(self.handle, name, 'mat4', value.handle)
    elseif Vec:isInstance(value) then
        assert(not type, 'Type argument is ignored, when called with a vector.')
        engine.SetLightUniform(self.handle, name, 'vec'..#value, value:unpack())
    else
        assert(type, 'Type is missing.')
        engine.SetLightUniform(self.handle, name, type, value)
    end
    self.activeUniforms[name] = true
end

--- Remove a uniform.
function Light:_unsetUniform( name )
    engine.UnsetLightUniform(self.handle, name)
    self.activeUniforms[name] = nil
end

function Light:_unsetAllUniforms()
    for name, _ in pairs(self.activeUniforms) do
        self:unsetUniform(name)
    end
end

function Light:_setTransformation( matrix )
    engine.SetLightTransformation(self.handle, matrix.handle)
end

function Light:_setAttachmentTarget( solidHandle, flags )
    engine.SetLightAttachmentTarget(self.handle, solidHandle, flags)
end


return Light
