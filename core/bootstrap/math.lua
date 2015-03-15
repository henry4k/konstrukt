--- @script core.bootstrap.math
--- Extensions to the standard @{math} functions.

-- luacheck: globals math

local floor = math.floor
local abs = math.abs
local min = math.min
local max = math.max


--- Test whether a number is an integer.
function math.isInteger( number )
    return floor(number) == number
end

--- Return the sign of a number.
--
-- - Returns `1` for positive numbers
-- - Returns `0` for zero.
-- - Returns `-1` for negative numbers.
--
function math.sign( x )
    if x ~= 0 then
        return x / abs(x)
    else
        return 0
    end
end

--- Test if a value is inside a given range.
function math.isBetween( value, minValue, maxValue )
    return value >= minValue and value <= maxValue
end

--- Limits/clamps a value to a range.
function math.boundBy( value, minValue, maxValue )
    return min(max(value, minValue), maxValue)
end
