local class  = require 'middleclass'
local engine = require 'apoapsis.engine'
local CreateAudioSource              = engine.CreateAudioSource
local DestroyAudioSource             = engine.DestroyAudioSource
local SetAudioSourceRelative         = engine.SetAudioSourceRelative
local SetAudioSourceLooping          = engine.SetAudioSourceLooping
local SetAudioSourcePitch            = engine.SetAudioSourcePitch
local SetAudioSourceGain             = engine.SetAudioSourceGain
local SetAudioSourceAttachmentTarget = engine.SetAudioSourceAttachmentTarget
local SetAudioSourceTransformation   = engine.SetAudioSourceTransformation
local EnqueueAudioBuffer             = engine.EnqueueAudioBuffer
local PlayAudioSource                = engine.PlayAudioSource
local PauseAudioSource               = engine.PauseAudioSource


--- Can play positional audio in an 3D environment.
-- Like the #AudioListener, the audio source has a transformation and an
-- attachment target. The attachment target provides information about position,
-- direction and velocity. If the audio source has no attachment target, those
-- properties are zero. The transformation matrix is applied additionaly after
-- the attachment targets position has been applied.
local AudioSource = class('apoapsis/core/AudioSource')

function AudioSource:initialize()
    self.handle = CreateAudioSource()
end

function AudioSource:destroy()
    DestroyAudioSource(self.handle)
    self.handle = nil
end

--- If set, the audio sources position is relative to the listeners position.
-- I.e. if you position it at (1,0,0) the sound will always be on the listeners
-- right side. No matter how the listener is positioned.
function AudioSource:setRelative( relative )
    SetAudioSourceRelative(self.handle, relative)
end

--- If set, the audio source will start from the beginning when the last enqueued buffer has been played.
function AudioSource:setLooping( looping )
    SetAudioSourceLooping(self.handle, looping)
end

function AudioSource:setPitch( pitch )
    SetAudioSourcePitch(self.handle, pitch)
end

function AudioSource:setGain( gain )
    SetAudioSourceGain(self.handle, gain)
end

function AudioSource:setAttachmentTarget( solid )
    SetAudioSourceAttachmentTarget(self.handle, solid.handle)
end

function AudioSource:setTransformation( matrix )
    SetAudioSourceTransformation(self.handle, matrix.handle)
end

function AudioSource:enqueue( buffer )
    EnqueueAudioBuffer(self.handle, buffer.handle)
end

--- Starts or continues to play the enqueued buffers.
function AudioSource:play()
    PlayAudioSource(self.handle)
end

function AudioSource:pause()
    PauseAudioSource(self.handle)
end


return AudioSource
