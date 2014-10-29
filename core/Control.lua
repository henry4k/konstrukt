local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Controllable      = require 'core/Controllable'
local RegisterControl  = ENGINE.RegisterControl
local SetEventCallback = ENGINE.SetEventCallback


local Control = {
    controlableStack = {}
}

function Control.register( name )
    RegisterControl(name)
end

function Control.pushControllable( controlable )
    assert(Object.includes(controlable.class, Controllable),
           'Must be called with an controlable.')
    local controlableStack = Control.controlableStack
    assert(not table.find(controlableStack, controlable),
           'A controlable may only be pushed once.')
    table.insert(controlableStack, controlable)
end

function Control.popControllable( controlable )
    assert(Object.includes(controlable.class, Controllable),
           'Must be called with an controlable.')
    local controlableStack = Control.controlableStack
    local i = table.find(controlableStack, controlable)
    assert(i, 'Controllable must have been pushed before removing.')
    table.remove(controlableStack, i)
end

local function onControlAction( name, absolute, delta )
    local controlableStack = Control.controlableStack
    local i = #controlableStack
    while i >= 1 do
        local controlable = controlableStack[i]
        if controlable:triggerControlEvent(name, absolute, delta) then
            return
        end
        i = i - 1
    end
end
SetEventCallback('ControlAction', onControlAction)


return Control
