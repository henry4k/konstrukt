local assert = assert
local class  = require 'middleclass'
local Object = class.Object
local Controlable = require 'core/Controlable'
local RegisterControl  = ENGINE.RegisterControl
local SetEventCallback = ENGINE.SetEventCallback


local Control = {
    controlableStack = {}
}

function Control.register( name )
    RegisterControl(name)
end

function Control.pushControlable( controlable )
    assert(Object.includes(controlable, Controlable),
           'Must be called with an controlable.')
    local controlableStack = Control.controlableStack
    assert(not table.find(controlableStack, controlable),
           'A controlable may only be pushed once.')
    table.insert(controlableStack, controlable)
end

function Control.removeControlable( controlable )
    assert(Object.includes(controlable, Controlable),
           'Must be called with an controlable.')
    local controlableStack = Control.controlableStack
    for i, v in ipairs(controlableStack) do
        if v == controlable then
            table.remove(controlableStack, i)
            return
        end
    end
end

local function onControlAction( name, value )
    local controlableStack = Control.controlableStack
    local i = #controlableStack
    while i >= 1 do
        local controlable = controlableStack[i]
        if controlable:triggerControlEvent(name, value) then
            return
        end
        i = i - 1
    end
end
SetEventCallback('ControlAction', onControlAction)


return Control
