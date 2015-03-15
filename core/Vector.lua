--- @classmod core.Vector
--- n-dimensional vectors!


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


--- Creates a new vector from arguments.
--
-- @function __call( ... )
--
-- @param[type=number] ...
--
-- @usage position = Vector(1,2,3)
--
function Vector:__call( ... )
    return setmetatable({...}, self.mt)
end

--- Tests if an object is a vector.
--
-- @usage assert(Vector:isInstance(position), 'Position must be a vector.')
--
function Vector:isInstance( v )
    return getmetatable(v) == self.mt
end

--- Create an independent copy of the instance.
function Vector.prototype:clone()
    return Vector(unpack(self))
end

--- Like @{table.unpack}, but for vectors.
--
-- Trying to unpack a 2d vector with 3 components, will automatically return
-- zero for the z axis.
--
-- @param[opt=#self] componentCount
-- Return exactly `componentCount` components.
--
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

--- Compute inverse vector length.
--
-- `v:inverseLength() == 1/v:length()`
--
function Vector.prototype:inverseLength()
    local result = 0
    for i = 1, #self do
        local component = self[i]
        result = result + component*component
    end
    return result
end

--- Vector length.
--
-- This is not equal to `#vector`!
-- While `#vector` provides the vectors component count, `vector:length()`
-- provides the vectors mathematical/geometrical length.
--
function Vector.prototype:length()
    return sqrt(self:inverseLength())
end

--- Computes the unit vector.
--
-- A unit vectors length is 1.  (Thats why its called unit vector.)
--
-- @return[type=core.Vector]
--
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

--- Test two vectors for equality.
function Vector.mt:__eq( other )
    for i = 1, max(#self, #other) do
        if not self[i] == other[i] then
            return false
        end
    end
    return true
end

function Vector.prototype:componentsLesserThan( other )
    for i = 1, max(#self, #other) do
        if not self[i] < other[i] then
            return false
        end
    end
    return true
end

function Vector.prototype:componentsGreaterThan( other )
    for i = 1, max(#self, #other) do
        if not self[i] > other[i] then
            return false
        end
    end
    return true
end

function Vector.prototype:componentsLesserOrEqualTo( other )
    for i = 1, max(#self, #other) do
        if not self[i] <= other[i] then
            return false
        end
    end
    return true
end

function Vector.prototype:componentsGreaterOrEqualTo( other )
    for i = 1, max(#self, #other) do
        if not self[i] >= other[i] then
            return false
        end
    end
    return true
end

--- Add another vector or number (scalar).  (Scalars affect all components.)
function Vector.mt:__add( other )
    return self:operate(other, function(a,b) return a+b end)
end

--- Substract another vector or number (scalar).  (Scalars affect all components.)
function Vector.mt:__sub( other )
    return self:operate(other, function(a,b) return a-b end)
end

--- Multiply with another vector or number (scalar).  (Scalars affect all components.)
function Vector.mt:__mul( other )
    return self:operate(other, function(a,b) return a*b end)
end

--- Divide by another vector or number (scalar).  (Scalars affect all components.)
function Vector.mt:__div( other )
    return self:operate(other, function(a,b) return a/b end)
end

-- Invert vector.
function Vector.mt:__unm()
    local r = Vector()
    for i = 1, #self do
        rawset(r, i, -self[i])
    end
    return r
end

--- Create a vectors string representation.
function Vector.mt:__tostring()
    return table.concat(self, ',')
end


return Vector
