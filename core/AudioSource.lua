local class = require 'core/middleclass'


--- Can play positional audio in an 3D environment.
-- Like the #AudioListener, the audio source has a transformation and an
-- attachment target. The attachment target provides information about position,
-- direction and velocity. If the audio source has no attachment target, those
-- properties are zero. The transformation matrix is applied additionaly after
-- the attachment targets position has been applied.
local AudioSource = class('core/AudioSource')

function AudioSource:initialize()
    self.handle = NATIVE.CreateAudioSource()
end

function AudioSource:destroy()
    NATIVE.DestroyAudioSouce(self.handle)
    self.handle = nil
end

--- If set, the audio sources position is relative to the listeners position.
-- I.e. if you position it at (1,0,0) the sound will always be on the listeners
-- right side. No matter how the listener is positioned.
function AudioSource:setRelative( relative )
    NATIVE.SetAudioSourceRelative(self.handle, relative)
end

--- If set, the audio source will start from the beginning when the last enqueued buffer has been played.
function AudioSource:setLooping( looping )
    NATIVE.SetAudioSourceLooping(self.handle, looping)
end

function AudioSource:setPitch( pitch )
    NATIVE.SetAudioSourcePitch(self.handle, pitch)
end

function AudioSource:setGain( gain )
    NATIVE.SetAudioSourceGain(self.handle, gain)
end

function AudioSource:setAttachmentTarget( solid )
    NATIVE.SetAudioSourceAttachmentTarget(self.handle, solid.handle)
end

function AudioSource:setTransformation( matrix )
    NATIVE.SetAudioSourceTransformation(self.handle, matrix.handle)
end

function AudioSource:enqueue( buffer )
    NATIVE.EnqueueAudioBuffer(self.handle, buffer.handle)
end

--- Starts or continues to play the enqueued buffers.
function AudioSource:play()
    NATIVE.PlayAudioSource(self.handle)
end

function AudioSource:pause()
    NATIVE.PauseAudioSource(self.handle)
end


return AudioSource
