--- Events can be used for inter object communication.
--
-- Using the event source mixin makes sense, if you can't know which objects
-- may want to react on an event.  Also you don't need to worry about
-- the targets life time, since it will automatically forget targets
-- that have no further references.
--
-- @mixin core.EventSource


local assert  = assert


local EventSource = {}

--- Must be called by the including class in its constructor.
function EventSource:initializeEventSource()
    self.events = {}
end

--- Must be called by the including class in its destructor.
function EventSource:destroyEventSource()
end

local eventMetaTable = { __mode = 'k' }

--- 
function EventSource:addEventTarget( eventName, target, callback )
    assert(eventName and target and callback)
    local event = self.events[eventName]
    if not event then
        event = setmetatable({}, eventMetaTable)
        self.events[eventName] = event
    end
    event[target] = callback
end

function EventSource:removeEventTarget( eventName, target )
    assert(eventName and target)
    local event = self.events[eventName]
    if event then
        event[target] = nil
    end
end

function EventSource:fireEvent( eventName, ... )
    local event = self.events[eventName]
    if event then
        for target, callback in pairs(event) do
            callback(target, ...)
        end
    end
end


return EventSource
