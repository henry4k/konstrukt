local class = require 'core/middleclass'


local AudioBuffer = class('core/AudioBuffer')

function AudioBuffer:initialize( fileName )
    self.handle = NATIVE.LoadAudioBuffer(fileName)
end


return AudioBuffer
