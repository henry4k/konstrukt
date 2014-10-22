local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Solid  = require 'core/Solid'
local Mat4   = require 'core/Matrix4'
local SetAudioListenerAttachmentTarget = ENGINE.SetAudioListenerAttachmentTarget
local SetAudioListenerTransformation   = ENGINE.SetAudioListenerTransformation


--- Describes the properties of your virtual ears.
-- Like the #AudioSource the listener has a transformation and an attachment
-- target. The attachment target provides information about position,
-- orientation and velocity. If the listener has no attachment target, those
-- properties are zero. The transformation matrix is applied additionaly after
-- the attachment targets position has been applied.
--
-- @see AudioListener.setAttachmentTarget
-- @see AudioListener.setTransformation
local AudioListener = {}


function AudioListener.setAttachmentTarget( solid, flags )
    assert(Object.isInstanceOf(solid, Solid), 'Attachment target must be a solid.')
    flags = flags or 'rt'
    SetAudioListenerAttachmentTarget(solid.handle, flags)
end

function AudioListener.setTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    SetAudioListenerTransformation(matrix.handle)
end


return AudioListener
