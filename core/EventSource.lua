--- @mixin core.EventSource
--- Events can be used for inter object communication.
--
-- Using the event source mixin makes sense, if you can't know which objects
-- may want to react on an event.  Also you don't need to worry about
-- the targets life time, since it will automatically forget targets
-- that have no further references.


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

--- Register callback for an event.
--
-- Only one callback is supported for each event/target combination.
--
-- @param eventName
-- @param target Passed as first parameter to the callback function.
-- @param callback
--
-- @usage
-- function FuelTank:onCollision( impulse, ... )
--     if impulse:length() > 3 then
--         self:explode()
--     end
-- end
-- solid:addEventTarget('collision', self, self.onCollision)
--
function EventSource:addEventTarget( eventName, target, callback )
    assert(eventName and target and callback)
    local event = self.events[eventName]
    if not event then
        event = setmetatable({}, eventMetaTable)
        self.events[eventName] = event
    end
    event[target] = callback
end

--- Unregisters a callback.
function EventSource:removeEventTarget( eventName, target )
    assert(eventName and target)
    local event = self.events[eventName]
    if event then
        event[target] = nil
    end
end

--- Call callbacks, which registered for an event.
--
-- @param eventName
-- @param ...
-- Arguments that are passed to the callback.
--
function EventSource:fireEvent( eventName, ... )
    local event = self.events[eventName]
    if event then
        for target, callback in pairs(event) do
            callback(target, ...)
        end
    end
end


return EventSource
