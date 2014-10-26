local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Solid  = require 'core/physics/Solid'
local Mat4   = require 'core/Matrix4'
local AudioBuffer = require 'core/audio/AudioBuffer'
local CreateAudioSource              = ENGINE.CreateAudioSource
local DestroyAudioSource             = ENGINE.DestroyAudioSource
local SetAudioSourceRelative         = ENGINE.SetAudioSourceRelative
local SetAudioSourceLooping          = ENGINE.SetAudioSourceLooping
local SetAudioSourcePitch            = ENGINE.SetAudioSourcePitch
local SetAudioSourceGain             = ENGINE.SetAudioSourceGain
local SetAudioSourceAttachmentTarget = ENGINE.SetAudioSourceAttachmentTarget
local SetAudioSourceTransformation   = ENGINE.SetAudioSourceTransformation
local EnqueueAudioBuffer             = ENGINE.EnqueueAudioBuffer
local PlayAudioSource                = ENGINE.PlayAudioSource
local PauseAudioSource               = ENGINE.PauseAudioSource


--- Can play positional audio in an 3D environment.
-- Like the #AudioListener, the audio source has a transformation and an
-- attachment target. The attachment target provides information about position,
-- direction and velocity. If the audio source has no attachment target, those
-- properties are zero. The transformation matrix is applied additionaly after
-- the attachment targets position has been applied.
local AudioSource = class('core/audio/AudioSource')

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
    assert(type(looping) == 'boolean')
    SetAudioSourceRelative(self.handle, relative)
end

--- If set, the audio source will start from the beginning when the last enqueued buffer has been played.
function AudioSource:setLooping( looping )
    assert(type(looping) == 'boolean')
    SetAudioSourceLooping(self.handle, looping)
end

function AudioSource:setPitch( pitch )
    assert(pitch >= 0, 'Pitch must be positive.')
    SetAudioSourcePitch(self.handle, pitch)
end

function AudioSource:setGain( gain )
    assert(gain >= 0, 'Gain should be positive.')
    SetAudioSourceGain(self.handle, gain)
end

function AudioSource:setAttachmentTarget( solid, flags )
    assert(Object.isInstanceOf(solid, Solid), 'Attachment target must be a solid.')
    flags = flags or 'rt'
    SetAudioSourceAttachmentTarget(self.handle, solid.handle, flags)
end

function AudioSource:setTransformation( matrix )
    assert(Object.isInstanceOf(matrix, Mat4), 'Transformation must be an matrix.')
    SetAudioSourceTransformation(self.handle, matrix.handle)
end

function AudioSource:enqueue( buffer )
    assert(Object.isInstanceOf(buffer, AudioBuffer), 'Wasn\'t called with an audio buffer.')
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
