local Control = {}

Control.keyCallbacks = {}
Control.axisCallbacks = {}

function Control.registerKey( name, callback )
    NATIVE.RegisterKeyControl(name)
    Control.keyCallbacks[name] = callback
end

function Control.registerAxis( name, callback )
    NATIVE.RegisterAxisControl(name)
    Control.axisCallbacks[name] = callback
end

local function onKeyAction( name, pressed )
    Control.keyCallbacks[name](pressed)
end
NATIVE.SetEventCallback('KeyControlAction', onKeyAction)

local function onAxisAction( name, absolute, delta )
    Control.axisCallbacks[name](absolute, delta)
end
NATIVE.SetEventCallback('AxisControlAction', onAxisAction)

return Control
