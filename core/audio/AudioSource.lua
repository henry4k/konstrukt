--- Can play positional audio in an 3D environment.
--
-- Like the @{AudioListener}, the audio source has a transformation and an
-- attachment target. The attachment target provides information about position,
-- direction and velocity. If the audio source has no attachment target, those
-- properties are zero. The transformation matrix is applied additionaly after
-- the attachment targets position has been applied.
--
-- Includes @{HasTransformation} and @{core.physics.HasAttachmentTarget}.
--
-- @classmod core.audio.AudioSource


local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Solid  = require 'core/physics/Solid'
local Mat4   = require 'core/Matrix4'
local AudioBuffer         = require 'core/audio/AudioBuffer'
local HasTransformation   = require 'core/HasTransformation'
local HasAttachmentTarget = require 'core/physics/HasAttachmentTarget'
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


local AudioSource = class('core/audio/AudioSource')
AudioSource:include(HasTransformation)
AudioSource:include(HasAttachmentTarget)

function AudioSource:initialize()
    self.handle = CreateAudioSource()
end

function AudioSource:destroy()
    DestroyAudioSource(self.handle)
    self.handle = nil
end

--- If set, the audio sources position is relative to the listeners position.
--
-- I.e. if you position it at (1,0,0) the sound will always be on the listeners
-- right side. No matter how the listener is positioned.
--
function AudioSource:setRelative( relative )
    assert(type(looping) == 'boolean')
    SetAudioSourceRelative(self.handle, relative)
end

--- If set, the audio source will start from the beginning when the last enqueued buffer has been played.
function AudioSource:setLooping( looping )
    assert(type(looping) == 'boolean')
    SetAudioSourceLooping(self.handle, looping)
end

--- Change sources pitch. (Make sounds higher or lower.)
function AudioSource:setPitch( pitch )
    assert(pitch >= 0, 'Pitch must be positive.')
    SetAudioSourcePitch(self.handle, pitch)
end

--- Change sources gain. (Make sounds louder or quieter.)
function AudioSource:setGain( gain )
    assert(gain >= 0, 'Gain should be positive.')
    SetAudioSourceGain(self.handle, gain)
end

--- Enqueues a buffer, which is played after all other enqueued buffers.
-- @param[type=AudioBuffer] buffer
function AudioSource:enqueue( buffer )
    assert(Object.isInstanceOf(buffer, AudioBuffer), 'Wasn\'t called with an audio buffer.')
    EnqueueAudioBuffer(self.handle, buffer.handle)
end

--- Starts or continues to play enqueued buffers.
function AudioSource:play()
    PlayAudioSource(self.handle)
end

--- Pauses playing enqueued buffers.
function AudioSource:pause()
    PauseAudioSource(self.handle)
end

function AudioSource:_setTransformation( matrix )
    SetAudioSourceTransformation(self.handle, matrix.handle)
end

function AudioSource:_setAttachmentTarget( solidHandle, flags )
    SetAudioSourceAttachmentTarget(self.handle, solidHandle, flags)
end


return AudioSource
