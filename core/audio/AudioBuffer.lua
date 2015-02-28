--- @classmod core.audio.AudioBuffer
--- An audio buffer contains audio data, which is e.g. loaded from a file.
--
-- Includes @{Resource}.


local class    = require 'middleclass'
local Resource = require 'core/Resource'
local LoadAudioBuffer    = ENGINE.LoadAudioBuffer
local DestroyAudioBuffer = ENGINE.DestroyAudioBuffer


local AudioBuffer = class('core/audio/AudioBuffer')
AudioBuffer:include(Resource)

--- Loads an audio source from a file.
--
-- @function static:load( fileName )
--
-- @return[type=AudioBuffer]
--
function AudioBuffer.static:_load( fileName )
    local buffer = AudioBuffer(fileName)
    return { value=buffer, destructor=buffer.destroy }
end

function AudioBuffer:initialize( fileName )
    self.handle = LoadAudioBuffer(fileName)
end

function AudioBuffer:destroy()
    DestroyAudioBuffer(self.handle)
    self.handle = nil
end


return AudioBuffer
