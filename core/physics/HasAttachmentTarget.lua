local assert  = assert
local class   = require 'middleclass'
local Object  = class.Object
local Solid   = require 'core/physics/Solid'


local HasAttachmentTarget = { static = {} }

function HasAttachmentTarget:included( targetClass, setAttachmentTargetFn )
    assert(setAttachmentTargetFn, 'Missing function to set attachment target.')
    targetClass.setAttachmentTarget_ = setAttachmentTargetFn
end

function HasAttachmentTarget:setAttachmentTarget( solid, flags )
    assert(Object.isInstanceOf(solid, Solid) or not solid, 'Attachment target must be a solid or nil.')
    flags = flags or 'rt'
    self.attachmentTarget = solid
    self:setAttachmentTarget_(solid, flags)
end

function HasAttachmentTarget:getAttachmentTarget()
    return self.attachmentTarget
end


return HasAttachmentTarget
