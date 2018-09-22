--- @mixin core.EffectTarget
--- Lets an object host @{core.Effect}s.


local class  = require 'middleclass'
local Object = class.Object
local Effect = require 'core/Effect'


local EffectTarget = {}

--- Must be called by the including class in its constructor.
function EffectTarget:initializeEffectTarget()
    self.effects = {}
end

--- Must be called by the including class in its destructor.
function EffectTarget:destroyEffectTarget()
    for _,effect in ipairs(self.effects) do
        effect:destroy()
    end
    self.effects = {}
end

--- Adds a new effect instance.
--
-- Constructs and adds an effect using the given class and the parameters.
--
-- @param[type=class] effectClass
--
-- @param ...
-- Parameters that are passed the effects constructor.
--
-- @return The effect instance.
--
function EffectTarget:addEffect( effectClass, ... )
    assert(Object.isSubclassOf(effectClass, Effect), 'The effect class must inherit Effect.')
    local effect = effectClass(self, ...)
    table.insert(self.effects, effect)
    table.sort(self.effects)
    return effect
end

--- Removes an existing effect.
--
-- Also calls the effects destructor upon successful removal.
--
-- @param[type=core.Effect] effect
--
function EffectTarget:removeEffect( effect )
    for i,e in ipairs(self.effects) do
        if e == effect then
            table.remove(self.effects, i)
            effect:destroy()
        end
    end
end

--- Calls a method in all hosted effects.
--
-- The order in which the effects are called is determined by their priority.
--
-- Effects may or may not have that method.
--
-- @param methodName
--
-- @param ...
-- Parameters passed to the method.
--
function EffectTarget:callEffects( methodName, ... )
    for _, effect in ipairs(self.effects) do
        local method = effect[methodName]
        if method then
            method(effect, ...)
        end
    end
end

--- Creates a list of all hosted effects, which are instances of the given class.
function EffectTarget:findEffectsByClass( effectClass )
    local found = {}
    for _, effect in ipairs(self.effects) do
        if effect:isInstanceOf(effectClass) then
            table.insert(found, effect)
        end
    end
    return found
end


return EffectTarget
