local m = {}

m.keyCallbacks = {}
m.axisCallbacks = {}

function m.registerKey( name, callback )
    NATIVE.RegisterKeyControl(name)
    m.keyCallbacks[name] = callback
end

function m.registerAxis( name, callback )
    NATIVE.RegisterAxisControl(name)
    m.axisCallbacks[name] = callback
end

local function onKeyAction( name, pressed )
    m.keyCallbacks[name](pressed)
end
NATIVE.SetEventCallback('KeyControlAction', onKeyAction)

local function onAxisAction( name, absolute, delta )
    m.axisCallbacks[name](absolute, delta)
end
NATIVE.SetEventCallback('AxisControlAction', onAxisAction)

return m
