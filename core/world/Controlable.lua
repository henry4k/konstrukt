local Controlable = {
    static = {
        controls = {}
    }
}


function Controlable.static:mapControl( controlName, method )
    local controls = self.static.controls
    assert(not controls[controlName], controlName..' has already been mapped!')
    controls[controlName] = method
end

function Controlable:triggerControlEvent( controlName, ... )
    local controls = self.class.controls
    local method = controls[controlName]
    if method then
        method(self, ...)
        return true
    else
        return false
    end
end


return Controlable
