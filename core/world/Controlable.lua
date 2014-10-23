local assert = assert
local type   = type
local Controlable = {}

function Controlable:initializeControlable()
    self.controls = {}
end

function Controlable:destroyControlable()
end

function Controlable:mapControl( controlName, methodName )
    self.controls[controlName] = methodName
end

function Controlable:triggerControlEvent( controlName, ... )
    local methodName = self.controls[controlName]
    if methodName then
        local method = self[methodName]
        assert(type(method) == 'function')
        method(self, ...)
    end
end


return Controlable
