local class    = require 'middleclass'
local Resource = require 'apoapsis.core.Resource'
local engine   = require 'apoapsis.engine'
local LoadAudioBuffer    = engine.LoadAudioBuffer
local DestroyAudioBuffer = engine.DestroyAudioBuffer


--- An audio buffer contains audio data, which is e.g. loaded from a file.
local AudioBuffer = class('apoapsis/core/AudioBuffer')
AudioBuffer:include(Resource)

function AudioBuffer.static:load( fileName )
    return AudioBuffer(fileName)
end

function AudioBuffer:initialize( fileName )
    self.handle = LoadAudioBuffer(fileName)
end

function AudioBuffer:destroy()
    DestroyAudioBuffer(self.handle)
    self.handle = nil
end


return AudioBuffer
