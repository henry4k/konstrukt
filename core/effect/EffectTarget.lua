local class = require 'core/middleclass'
local Object = class.Object
local Effect = require 'core/effect/Effect'


local EffectTarget = {}

function EffectTarget:initializeEffectTarget()
    self.effects = {}
end

function EffectTarget:destroyEffectTarget()
    for _,effect in ipairs(self.effects) do
        effect:destroy()
    end
end

function EffectTarget:addEffect( effectClass, ... )
    assert(Object.isSubclassOf(effectClass, Effect), 'effectClass must inherit Effect.')
    local effect = effectClass:new(self, ...)
    table.insert(self.effects, effect)
    table.sort(self.effects)
    return effect
end

function EffectTarget:removeEffect( effect )
    for i,e in ipairs(self.effects) do
        if e == effect then
            table.remove(self.effects, i)
            effect:destroy()
        end
    end
end

function EffectTarget:callEffects( event, ... )
    for _,effect in ipairs(self.effects) do
        local eventHandler = effect[event]
        if eventHandler then
            eventHandler(effect, ...)
        end
    end
end

function EffectTarget:findEffectsByClass( effectClass )
    local found = {}
    for _,effect in ipairs(self.effects) do
        if e:isInstanceOf(effectClass) then
            table.insert(found, effect)
        end
    end
    return found
end


return EffectTarget
