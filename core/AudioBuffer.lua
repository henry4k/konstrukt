local m = {}
m.__index = m

function m.newFromHandle( handle )
    local self = setmetatable({}, m)
    self.handle = handle
    return self
end

function m.new( fileName )
    return m.newFromHandle(NATIVE.LoadAudioBuffer(fileName))
end

return m
