--- @module core.graphics.LightWorld
-- Stores light sources, which illuminate a scene.


local engine = require 'engine'
local class  = require 'middleclass'
local Scheduler         = require 'core/Scheduler'
local ShaderVariableSet = require 'core/graphics/ShaderVariableSet'


local LightWorld = class('core/graphics/LightWorld')

function LightWorld:initialize( lightCountUniformName,
                                lightPositionUniformName )
    Scheduler.awaitCall(function()
        self.handle = engine.CreateLightWorld(lightCountUniformName,
                                              lightPositionUniformName)
        self.shaderVariables =
            ShaderVariableSet(engine.GetLightWorldShaderVariableSet(self.handle))
        self.unusedLightShaderVariables =
            ShaderVariableSet(engine.GetLightWorldUnusedLightShaderVariableSet(self.handle))
    end)
end

function LightWorld:destroy()
    Scheduler.blindCall(engine.DestroyLightWorld, self.handle)
    self.handle = nil
    self.shaderVariables:destroy()
    self.shaderVariables = nil
    self.unusedLightShaderVariables:destroy()
    self.unusedLightShaderVariables = nil
end

function LightWorld:setMaxActiveLightCount( count )
    assert(count > 0 and math.isInteger(count),
           'Count must be a positive integer.')
    Scheduler.blindCall(engine.SetMaxActiveLightCount, self.handle, count)
end

--- Creates a new light in this light world.
-- @return[type=core.graphics.Light]
function LightWorld:createLight( typeName, ... )
    error('Implementation missing.')
end

function LightWorld:_createLight( klass, ... )
    local handle = Scheduler.awaitCall(engine.CreateLight, self.handle, klass.lightType)
    local instance = klass:allocate()
    instance.handle = handle
    instance:initialize(...)
    return instance
end


return LightWorld
