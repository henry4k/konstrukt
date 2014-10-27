local assert  = assert


--- Events can be used for iter object communication.
-- Using the vent source mixin makes sense, if can't know which objects may wan't
-- to react on an event.  Also you don't need to worry about the targets life time,
-- since EventSource will automatically forget about targets without any further references.
local EventSource = {}

function EventSource:initializeEventSource()
    self.events = {}
end

function EventSource:destroyEventSource()
end

local eventMetaTable = { __mode = 'k' }

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