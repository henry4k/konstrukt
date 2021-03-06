--- @classmod core.audio.AudioSource
--- Can play positional audio in an 3D environment.
--
-- Like the @{core.audio.AudioListener}, the audio source has a transformation
-- and an attachment target. The attachment target provides information about
-- position, direction and velocity.  If the audio source has no attachment target,
-- those properties are zero.  The transformation matrix is applied additionaly
-- after the attachment targets position has been applied.
--
-- Includes @{core.HasTransformation} and @{core.physics.HasAttachmentTarget}.


local engine = require 'engine'
local class  = require 'middleclass'
local Object = class.Object
local Scheduler           = require 'core/Scheduler'
local AudioBuffer         = require 'core/audio/AudioBuffer'
local HasTransformation   = require 'core/HasTransformation'
local HasAttachmentTarget = require 'core/physics/HasAttachmentTarget'


local AudioSource = class('core/audio/AudioSource')
AudioSource:include(HasTransformation)
AudioSource:include(HasAttachmentTarget)

function AudioSource:initialize()
    self.handle = Scheduler.awaitCall(engine.CreateAudioSource)
end

function AudioSource:destroy()
    Scheduler.blindCall(engine.DestroyAudioSource, self.handle)
    self.handle = nil
end

--- If set, the audio sources position is relative to the listeners position.
--
-- I.e. if you position it at (1,0,0) the sound will always be on the listeners
-- right side. No matter how the listener is positioned.
--
function AudioSource:setRelative( relative )
    assert(type(relative) == 'boolean')
    Scheduler.blindCall(engine.SetAudioSourceRelative, self.handle, relative)
end

--- If set, the audio source will start from the beginning when the last enqueued buffer has been played.
function AudioSource:setLooping( looping )
    assert(type(looping) == 'boolean')
    Scheduler.blindCall(engine.SetAudioSourceLooping, self.handle, looping)
end

--- Change sources pitch. (Make sounds higher or lower.)
function AudioSource:setPitch( pitch )
    assert(pitch >= 0, 'Pitch must be positive.')
    Scheduler.blindCall(engine.SetAudioSourcePitch, self.handle, pitch)
end

--- Change sources gain. (Make sounds louder or quieter.)
function AudioSource:setGain( gain )
    assert(gain >= 0, 'Gain should be positive.')
    Scheduler.blindCall(engine.SetAudioSourceGain, self.handle, gain)
end

--- Enqueues a buffer, which is played after all other enqueued buffers.
-- @param[type=core.AudioBuffer] buffer
function AudioSource:enqueue( buffer )
    assert(Object.isInstanceOf(buffer, AudioBuffer), 'Wasn\'t called with an audio buffer.')
    Scheduler.blindCall(engine.EnqueueAudioBuffer, self.handle, buffer.handle)
end

--- Starts or continues to play enqueued buffers.
function AudioSource:play()
    Scheduler.blindCall(engine.PlayAudioSource, self.handle)
end

--- Pauses playing enqueued buffers.
function AudioSource:pause()
    Scheduler.blindCall(engine.PauseAudioSource, self.handle)
end

function AudioSource:_setTransformation( matrix )
    Scheduler.blindCall(engine.SetAudioSourceTransformation, self.handle, matrix.handle)
end

function AudioSource:_setAttachmentTarget( solidHandle, flags )
    Scheduler.blindCall(engine.SetAudioSourceAttachmentTarget, self.handle, solidHandle, flags)
end


return AudioSource
