local m = {}
m.__index = m

function m.newFromHandle( handle )
    local self = setmetatable({}, m)
    self.handle = handle
    return self
end

function m.new( triggerCallback )
    return m.newFromHandle(NATIVE.CreateAudioSource(triggerCallback or false))
end

function m:setRelative( relative )
    NATIVE.SetAudioSourceRelative(self.handle, relative)
end

function m:setLooping( looping )
    NATIVE.SetAudioSourceLooping(self.handle, looping)
end

function m:setPitch( pitch )
    NATIVE.SetAudioSourcePitch(self.handle, pitch)
end

function m:setGain( gain )
    NATIVE.SetAudioSourceGain(self.handle, gain)
end

function m:enqueue( buffer )
    NATIVE.EnqueueAudioBuffer(self.handle, buffer.handle)
end

function m:play()
    NATIVE.PlayAudioSource(self.handle)
end

function m:pause()
    NATIVE.PauseAudioSource(self.handle)
end

function m:free()
    NATIVE.FreeAudioSource(self.handle)
end

return m
