Callbacks <- {}

// --- key controls ---

function RegisterKeyControl( name, callback )
{
    if(::native.RegisterKeyControl(name))
        Callbacks[name] <- callback
}

function OnKeyControlAction( name, pressed )
{
    Callbacks[name](pressed)
}
::native.RegisterKeyControlCallback(OnKeyControlAction)


// --- axis controls ---

function RegisterAxisControl( name, callback )
{
    if(::native.RegisterAxisControl(name))
        Callbacks[name] <- callback
}

function OnAxisControlAction( name, absolute, delta )
{
    Callbacks[name](absolute, delta)
}
::native.RegisterAxisControlCallback(OnAxisControlAction)


return {
    "RegisterKeyControl": RegisterKeyControl,
    "RegisterAxisControl": RegisterAxisControl
}
