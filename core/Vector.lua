local assert       = assert
local setmetatable = setmetatable
local getmetatable = getmetatable
local rawset       = rawset
local max          = math.max
local sqrt         = math.sqrt
local unpack       = table.unpack


local Vector =
{
    mt = {},
    prototype = {}
}
Vector.mt.__index = Vector.prototype
setmetatable(Vector.prototype, Vector.prototype)
setmetatable(Vector, Vector)


function Vector:__call( ... )
    return setmetatable({...}, self.mt)
end

function Vector:isInstance( v )
    return getmetatable(v) == self.mt
end

function Vector.prototype:clone()
    return Vector(unpack(self))
end

function Vector.prototype:unpack( componentCount )
    if #self == componentCount then
        return unpack(self)
    else
        local r = {}
        for i = 1, componentCount do
            r[i] = self[i]
        end
        return unpack(r)
    end
end

function Vector.prototype:operate( other, operationFn )
    local r = Vector()
    if Vector:isInstance(other) then
        for i = 1, max(#self, #other) do
            rawset(r, i, operationFn(self[i], other[i]))
        end
    else -- treat as single value
        for i = 1, #self do
            rawset(r, i, operationFn(self[i], other))
        end
    end
    return r
end

function Vector.prototype:inverseLength()
    local result = 0
    for i = 1, #self do
        local component = self[i]
        result = result + component*component
    end
    return result
end

function Vector.prototype:length()
    return sqrt(self:inverseLength())
end

function Vector.prototype:normalize()
    local length = self:length()
    local result = self:clone()
    for i = 1, #result do
        local component = result[i]
        result[i] = component / length
    end
    return result
end

function Vector.prototype:__index( key )
    if type(key) == 'number' then
        return 0
    else
        return nil
    end
end

function Vector.mt:__eq( other )
    for i = 1, max(#self, #other) do
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
    local r = Vector()
    for i = 1, #self do
        rawset(r, i, -self[i])
    end
    return r
end

function Vector.mt:__tostring()
    return table.concat(self, ',')
end


return Vector
