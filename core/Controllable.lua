--- @module core.Controllable

---
-- @mixin Controllable
Controllable = {
    static = {}
}

---
-- @local
function Controllable:included( klass )
    klass.static.controls = {}
end

---
-- @param controlName
-- @param method
function Controllable.static:mapControl( controlName, method )
    local controls = self.static.controls
    assert(not controls[controlName], controlName..' has already been mapped!')
    controls[controlName] = method

    local Control = require 'core/Control'
    Control.register(controlName)
end

---
-- Is called by `Control`.
-- @callback
-- @param controlName
-- @param ... Parameters which passed to the controls method.
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
