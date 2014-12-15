---
--
-- @see Solid
--
-- @module core.physics.HasAttachmentTarget


local assert  = assert
local class   = require 'middleclass'
local Object  = class.Object
local Solid   = require 'core/physics/Solid'


local HasAttachmentTarget = {}

---
-- @param solid
-- @param flags
function HasAttachmentTarget:setAttachmentTarget( solid, flags )
    assert(Object.isInstanceOf(solid, Solid) or not solid, 'Attachment target must be a solid or nil.')
    flags = flags or 'rt'
    self.attachmentTarget = solid
    if solid then
        self:_setAttachmentTarget(solid.handle, flags)
    else
        self:_setAttachmentTarget(nil, '')
    end
end

---
function HasAttachmentTarget:getAttachmentTarget()
    return self.attachmentTarget
end


return HasAttachmentTarget
