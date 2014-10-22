local assert = assert
local RegisterKeyControl  = ENGINE.RegisterKeyControl
local RegisterAxisControl = ENGINE.RegisterAxisControl
local SetEventCallback    = ENGINE.SetEventCallback


local Control = {}

Control.keyCallbacks = {}
Control.axisCallbacks = {}

function Control.registerKey( name, callback )
    assert(type(callback) == 'function', 'Callback must be a function.')
    RegisterKeyControl(name)
    Control.keyCallbacks[name] = callback
end

function Control.registerAxis( name, callback )
    assert(type(callback) == 'function', 'Callback must be a function.')
    RegisterAxisControl(name)
    Control.axisCallbacks[name] = callback
end

local function onKeyAction( name, pressed )
    Control.keyCallbacks[name](pressed)
end
SetEventCallback('KeyControlAction', onKeyAction)

local function onAxisAction( name, absolute, delta )
    Control.axisCallbacks[name](absolute, delta)
end
SetEventCallback('AxisControlAction', onAxisAction)

return Control
