local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Controlable      = require 'core/Controlable'
local RegisterControl  = ENGINE.RegisterControl
local SetEventCallback = ENGINE.SetEventCallback


local Control = {
    controlableStack = {}
}

function Control.register( name )
    RegisterControl(name)
end

function Control.pushControlable( controlable )
    assert(Object.includes(controlable.class, Controlable),
           'Must be called with an controlable.')
    local controlableStack = Control.controlableStack
    assert(not table.find(controlableStack, controlable),
           'A controlable may only be pushed once.')
    table.insert(controlableStack, controlable)
end

function Control.popControlable( controlable )
    assert(Object.includes(controlable.class, Controlable),
           'Must be called with an controlable.')
    local controlableStack = Control.controlableStack
    local i = table.find(controlableStack, controlable)
    assert(i, 'Controlable must have been pushed before removing.')
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
