local taggedcoro = require'taggedcoro'
local class = require'middleclass'


local Coroutine = class'core/Coroutine'

function Coroutine.static:fromHandle(handle)
    assert(taggedcoro.tag(handle) == self)
    local instance = self:allocate()
    instance.handle = handle
    return instance
end

function Coroutine.static:yield()
    return taggedcoro.yield(self)
end

function Coroutine.static:running()
    local coro = taggedcoro.running()
    while taggedcoro.tag(coro) ~= self do
        coro = taggedcoro.parent(coro)
        if not coro then
            return nil, 'Not in a coroutine of type '..self.name
        end
    end
    return self:fromHandle(coro)
end

function Coroutine:initialize(fn)
    self.handle = taggedcoro.create(self.class, fn)
end

function Coroutine:resume(...)
    return taggedcoro.resume(self.handle, ...)
end

function Coroutine:call(...)
    return taggedcoro.call(self.handle, ...)
end

Coroutine.__call = Coroutine.call

return Coroutine
