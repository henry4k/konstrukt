--- @module core.Control
--- Manages @{core.Control}s and @{core.Controllable}s.
-- 
-- Each control is identified by a name and must be registered before use.
-- Controls are mapped to input devices using configuration values.
--
-- E.g. when registering a control called `jump`, the engine reads the config
-- key `control.jump` to determine the mapped input device.
-- If `control.jump` was set to `space`, each time you press or release the
-- space key on your keyboard triggers a control action.
--
-- Besides the keyboard, there are also other input devices:
-- 
-- Mouse buttons can be referenced using `mouse-button:0`, `mouse-button:1`, etc.
-- Where 0 is the primary button, 1 the secondary button, and so on.
--
-- Mouse axes are handled the same way:  `mouse:0` refers to the x-axis and
-- `mouse:1` to the y-axis.  The mouse wheel is called `mouse-wheel:0` and `mouse-wheel:1`.
-- (Thats because some mice have scrollballs, which provide x and y axes.)
--
-- Joysticks have buttons and axes, which work like a mouse, except that there
-- can be multiple joysticks:  e.g. `joystick0-button:0` refers to the first
-- button on the first joystick.
--
-- Controllables are stored in a stack.
-- Each time a control changes its value, the first controllable which maps
-- that control receives the event.


local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Controllable     = require 'core/Controllable'
local RegisterControl  = ENGINE.RegisterControl
local SetEventCallback = ENGINE.SetEventCallback


local Control = {
    controllableStack = {}
}

--- Register a control, which can then be used by @{core.Controllable}s.
function Control.register( name )
    RegisterControl(name)
end

--- Places a controllable on top of the stack.
--
-- Each controllable may only be pushed once!
--
-- @param[type=core.Controllable] controllable
--
function Control.pushControllable( controllable )
    assert(Object.includes(controllable.class, Controllable),
           'Must be called with an controllable.')
    local controllableStack = Control.controllableStack
    assert(not table.find(controllableStack, controllable),
           'A controllable may only be pushed once.')
    table.insert(controllableStack, controllable)
end

--- Removes a controllale from the stack.
--
-- The controllable must be on top!
--
-- @param[type=core.Controllable] controllable
--
function Control.popControllable( controllable )
    assert(Object.includes(controllable.class, Controllable),
           'Must be called with an controllable.')
    local controllableStack = Control.controllableStack
    local i = table.find(controllableStack, controllable)
    assert(i, 'Controllable must have been pushed before removing.')
    table.remove(controllableStack, i)
end

local function onControlAction( name, absolute, delta )
    local controllableStack = Control.controllableStack
    local i = #controllableStack
    while i >= 1 do
        local controllable = controllableStack[i]
        if controllable:triggerControlEvent(name, absolute, delta) then
            return
        end
        i = i - 1
    end
end
SetEventCallback('ControlAction', onControlAction)


return Control
