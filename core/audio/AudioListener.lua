--- @module core.audio.AudioListener
--- Describes the properties of your virtual ears.
--
-- Like the @{core.audio.AudioSource} the listener has a transformation and an
-- attachment target. The attachment target provides information about position,
-- orientation and velocity. If the listener has no attachment target, those
-- properties are zero. The transformation matrix is applied additionaly after
-- the attachment targets position has been applied.


local engine    = require 'engine'
local class     = require 'middleclass'
local Scheduler = require 'core/Scheduler'
local Object    = class.Object
local Solid     = require 'core/physics/Solid'
local Mat4      = require 'core/Matrix4'


local AudioListener = {}


--- Attaches the listener to a solid.
--
-- @param[type=core.physics.Solid] solid
-- @param[type=string,opt='rt'] flags
-- See @{core.physics.HasAttachmentTarget:setAttachmentTarget}.
--
function AudioListener.setAttachmentTarget( solid, flags )
    assert(Object.isInstanceOf(solid, Solid), 'Attachment target must be a solid.')
    flags = flags or 'rt'
    Scheduler.blindCall(engine.SetAudioListenerAttachmentTarget, solid.handle, flags)
end

--- Change the transformation matrix.
--
-- @param[type=core.Matrix4] matrix
--
function AudioListener.setTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    Scheduler.blindCall(engine.SetAudioListenerTransformation, matrix.handle)
end


return AudioListener
