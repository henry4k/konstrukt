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


function AudioListener.setAttachmentTarget( solid )
    SetAudioListenerAttachmentTarget(solid.handle)
end

function AudioListener.setTransformation( matrix )
    SetAudioListenerTransformation(matrix.handle)
end


return AudioListener
