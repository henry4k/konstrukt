--- @classmod core.Timer
--- Repeatedly calls a function after a given timeframe has been simulated by
--- the engine.
--
-- @usage
-- function Grenade:initialize()
--     self.timer = Timer(10, self, Grenade.onTimerTriggered)
-- end
-- function Grenade:onTimerTriggered( timeDelta, timer )
--     timer:destroy()
--     self.timer = nil
--     self:explode()
-- end


local engine    = require 'engine'
local class     = require 'middleclass'
local Scheduler = require 'core/Scheduler'


local timerHandleToInstanceMap = {}

local function onTimerTriggered( timer, timeDelta )
    local timerInstance = timerHandleToInstanceMap[timer]
    assert(timerInstance, 'Unknown timer instance.')
    local target = timerInstance.target
    if target then
        timerInstance.callback(target, timeDelta, timerInstance)
    else
        timerInstance.callback(timeDelta, timerInstance)
    end
end
Scheduler.blindCall(engine.SetEventCallback, 'TimerTriggered', onTimerTriggered)


local Timer = class('core/Timer')

--- Allocates a new timer object.
--
-- It's safe to call this inside a timer callback.
--
-- @param[type=number] minDelay
-- The implementation guarantees that at least `minDelay` seconds must pass,
-- till the callback is triggered.
--
-- @param target
-- Arbitrary user data which can be passed through to the callback as first
-- parameter, if given (i.e. not nil).
--
-- @param[type=function] callback
-- Function which is called whenever the timer has been triggered.
--
function Timer:initialize( minDelay, target, callback )
    self.target = target
    self.callback = callback
    self.handle = Scheduler.awaitCall(engine.CreateTimer, minDelay)
    timerHandleToInstanceMap[self.handle] = self
end

--- Invalidates a timer object.
-- It's safe to call this inside a timer callback.
function Timer:destroy()
    assert(self.handle)
    Scheduler.blindCall(engine.DestroyTimer, self.handle)
    timerHandleToInstanceMap[self.handle] = nil
    self.handle = nil
end


return Timer
