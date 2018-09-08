--- @classmod core.world.FreeEntity


local class        = require 'middleclass'
local EffectTarget = require 'core/EffectTarget'
local Entity       = require 'core/world/Entity'


local FreeEntity = class('core/world/FreeEntity')
FreeEntity:include(EffectTarget)
FreeEntity:include(Entity)

function FreeEntity:initialize()
    self:initializeEffectTarget()
    self:initializeEntity()
end

function FreeEntity:destroy()
    self:destroyEntity()
    self:destroyEffectTarget()
end

function FreeEntity:setAttachmentTarget( target )
    self.attachmentTarget = target
    for attachable, _ in pairs(self.attachments) do
        attachable:setAttachmentTarget(target)
    end
end

return FreeEntity
