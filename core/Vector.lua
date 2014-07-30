local Vector =
{
    mt = {},
    prototype = {}
}
Vector.mt.__index = Vector.prototype
setmetatable(Vector.prototype, Vector.prototype)


function Vector:new( ... )
    return setmetatable({...}, self.mt)
end

function Vector:isInstance( v )
    return getmetatable(v) == self.mt
end

function Vector.prototype:operate( other, operationFn )
    local r = Vector:new()
    for i = 1, math.max(#self, #other) do
        rawset(r, i, operationFn(self[i], other[i]))
    end
    return r
end

function Vector.prototype:__index( key )
    return 0
end

function Vector.mt:__eq( other )
    for i = 1, math.max(#self, #other) do
        if self[i] ~= other[i] then
            return false
        end
    end
    return true
end

function Vector.mt:__add( other )
    return self:operate(other, function(a,b) return a+b end)
end

function Vector.mt:__sub( other )
    return self:operate(other, function(a,b) return a-b end)
end

function Vector.mt:__mul( other )
    return self:operate(other, function(a,b) return a*b end)
end

function Vector.mt:__div( other )
    return self:operate(other, function(a,b) return a/b end)
end

function Vector.mt:__unm()
    local r = Vector:new()
    for i = 1, #self do
        rawset(r, i, -self[i])
    end
    return r
end

function Vector.mt:__tostring()
    return table.concat(self, ',')
end


return Vector