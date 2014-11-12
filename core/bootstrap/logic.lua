local floor = math.floor
local min = math.min
local max = math.max

function math.isInteger( number )
    return floor(number) == number
end

function math.isBetween( value, min, max )
    return value >= min and value <= max
end

function math.boundBy( value, minValue, maxValue )
    return min(max(value, minValue), maxValue)
end
