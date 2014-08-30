local class = require 'core/middleclass'


--- An audio buffer contains audio data, which is e.g. loaded from a file.
local AudioBuffer = class('core/AudioBuffer')

function AudioBuffer:initialize( fileName )
    self.handle = NATIVE.LoadAudioBuffer(fileName)
end

function AudioBuffer:destroy()
    NATIVE.DestroyAudioBuffer(self.handle)
    self.handle = nil
end


return AudioBuffer
