--- @classmod core.audio.AudioBuffer
--- An audio buffer contains audio data, which is e.g. loaded from a file.
--
-- Includes @{core.Resource}.


local engine    = require 'engine'
local class     = require 'middleclass'
local Scheduler = require 'core/Scheduler'
local Resource  = require 'core/Resource'


local AudioBuffer = class('core/audio/AudioBuffer')
AudioBuffer:include(Resource)

--- Loads an audio source from a file.
--
-- @function static:load( fileName )
--
-- @return[type=core.audio.AudioBuffer]
--
function AudioBuffer.static:_load( fileName )
    local buffer = AudioBuffer(fileName)
    return { value=buffer, destructor=buffer.destroy }
end

function AudioBuffer:initialize( fileName )
    self.handle = Scheduler.awaitCall(engine.LoadAudioBuffer, fileName)
end

function AudioBuffer:destroy()
    Scheduler.blindCall(engine.DestroyAudioBuffer, self.handle)
    self.handle = nil
end


return AudioBuffer
