--- @module core.graphics.LightWorld
-- Stores light sources, which illuminate a scene.


local engine = require 'engine'
local class  = require 'middleclass'


local LightWorld = class('core/graphics/LightWorld')
local ShaderVariableSet = require 'core/graphics/ShaderVariableSet'

function LightWorld:initialize( lightCountUniformName,
                                lightPositionUniformName )
    self.handle = engine.CreateLightWorld(lightCountUniformName,
                                          lightPositionUniformName)
    self.shaderVariables =
        ShaderVariableSet(engine.GetLightWorldShaderVariableSet(self.handle))
end

function LightWorld:destroy()
    engine.DestroyLightWorld(self.handle)
    self.handle = nil
    self.shaderVariables:destroy()
    self.shaderVariables = nil
end

function LightWorld:setMaxActiveLightCount( count )
    assert(count > 0 and math.isInteger(count),
           'Count must be a positive integer.')
    engine.SetMaxActiveLightCount(self.handle, count)
end

--- Creates a new light in this light world.
-- @return[type=core.graphics.Light]
function LightWorld:createLight( typeName, ... )
    error('Implementation missing.')
end

function LightWorld:_createLight( klass, ... )
    local handle = engine.CreateLight(self.handle, klass.lightType)
    local instance = klass:allocate()
    instance.handle = handle
    instance:initialize(...)
    return instance
end


return LightWorld
