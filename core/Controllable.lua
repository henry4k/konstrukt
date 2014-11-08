local Controllable = {
    static = {}
}

function Controllable:included( klass )
    klass.static.controls = {}
end

function Controllable.static:mapControl( controlName, method )
    local controls = self.static.controls
    assert(not controls[controlName], controlName..' has already been mapped!')
    controls[controlName] = method

    local Control = require 'core/Control'
    Control.register(controlName)
end

function Controllable:triggerControlEvent( controlName, ... )
    local controls = self.class.controls
    local method = controls[controlName]
    if method then
        method(self, ...)
        return true
    else
        return false
    end
end


return Controllable
