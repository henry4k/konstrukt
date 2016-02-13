--- @mixin core.world.Entity
--- Something in the game world, which you can see and interact with.


local EffectTarget = require 'core/EffectTarget'


local Entity = {}

function Entity:included( klass )
    assert(klass:includes(EffectTarget), 'EffectTarget mixin required.')
end

--- Must be called by the including class in its constructor.
function Entity:initializeEntity()
    self.attachments = {} -- k/v pairs (see Entity:attach)
end

--- Must be called by the including class in its destructor.
function Entity:destroyEntity()
    for attachable, _ in pairs(self.attachments) do
        self:detach(attachable)
    end
end

function Entity:attach( attachable, transformation )
    assert(not self.attachments[attachable], 'This has already been attached.')
    assert(attachable.setAttachmentTarget, 'setAttachmentTarget missing')
    assert(attachable.setTransformation, 'setTransformation missing')
    self.attachments[attachable] = transformation or true
    attachable:setAttachmentTarget(self.attachmentTarget)
    if transformation then
        attachable:setTransformation(transformation)
    end
end

function Entity:detach( attachable )
    assert(self.attachments[attachable], 'This has not been attached yet.')
    self.attachments[attachable] = nil
    attachable:setAttachmentTarget(nil)
    attachable:setTransformation(nil)
end

return Entity
