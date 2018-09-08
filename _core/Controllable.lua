--- @mixin core.Controllable
--- For things that want to listen for specific control events.

local Controllable = {
    static = {}
}

function Controllable:included( klass )
    klass.static.controls = {}
end

--- Must be called by the including class in its constructor.
function Controllable:initializeControllable()
end

--- Must be called by the including class in its destructor.
function Controllable:destroyControllable()
end

function Controllable.static:getControls()
    local controls = self.controls
    if not controls then
        controls = {}
        self.controls = controls
    end
    return controls
end

--- Map the given control to a method.
--
-- Implicitly registers the control.
--
-- @param controlName
-- @param[type=function] method
-- @see core.Control.pushControllable
--
function Controllable.static:mapControl( controlName, method )
    local controls = self.static:getControls()
    assert(not controls[controlName], controlName..' has already been mapped!')
    controls[controlName] = method

    local Control = require 'core/Control'
    Control.register(controlName)
end

function Controllable:setChildControllables( childs )
    self.childControllables = childs
end

--- Is called by @{core.Control}.
-- @internal
-- @param controlName
-- @param ...
-- Parameters which passed to the controls method.
function Controllable:triggerControlEvent( controlName, ... )
    local childControllables = self.childControllables
    if childControllables then
        for _, child in ipairs(childControllables) do
            local result = child:triggerControlEvent(controlName, ...)
            if result == true then
                return true
            end
        end
    end

    local controls = self.class:getControls()
    local method = controls[controlName]
    if method then
        method(self, ...)
        return true
    else
        return false
    end
end

return Controllable
