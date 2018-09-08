--- @module core.GlobalEventSource
--- Like the @{core.EventSource} mixin, but for global events.


local class       = require 'middleclass'
local EventSource = require 'core/EventSource'


local GlobalEventSource = class('core/GlobalEventSource')
GlobalEventSource:include(EventSource)

function GlobalEventSource:initialize()
    self:initializeEventSource()
end

function GlobalEventSource:destroy()
    self:destroyEventSource()
end


return GlobalEventSource()
