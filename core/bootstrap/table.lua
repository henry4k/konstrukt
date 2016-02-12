--- @script core.bootstrap.table
--- Extensions to the standard @{table} functions.

-- luacheck: globals table

local ipairs = ipairs
local pairs = pairs
local setmetatable = setmetatable
local getmetatable = getmetatable
local random = math.random
local sort = table.sort

--- Copy entries of a set of tables into the target table.
--
-- The targets metatable is not modified.
-- Remember that this is just a shallow copy.
--
-- @return
-- The target table.
--
function table.include( target, ... )
    for _,includedTable in ipairs({...}) do
        for key, value in pairs(includedTable) do
            target[key] = value
        end
    end
    return target
end
local include = table.include

--- Duplicates the target table and returns the copy.
-- The copy has the same metatable like the target.
function table.copy( target )
    local newTable = setmetatable({}, getmetatable(target))
    return include(newTable, target)
end

--- Join multiple tables into one.
-- The second table overwrites keys from the first table and so on.
function table.merge( ... )
    return include({}, ...)
end

--- Searches `value` in `target` and returns its key or nil if nothing was found.
function table.find( target, value )
    for k,v in pairs(target) do
        if v == value then
            return k
        end
    end
end

local function weightComparision( a, b )
    return a.weight < b.weight
end

--- Sort a table by computing a weight for each entry.
function table.sortByWeight( target, weightingFn )
    local entriesWithWeighting = {}
    for i, entry in ipairs(target) do
        local weight = weightingFn(entry)
        entriesWithWeighting[i] = { entry=entry, weight=weight }
    end

    sort(entriesWithWeighting, weightComparision)

    for i, v in ipairs(entriesWithWeighting) do
        target[i] = v.entry
    end
end
local sortByWeight = table.sortByWeight

local function randomWeight( entry )
    return random()
end

--- Order the array part of a table randomly.
function table.shuffle( target )
    return sortByWeight(target, randomWeight)
end

--- Gather all keys in a table and return them as a list.
function table.listKeys( kvTable )
    local result = {}
    local i = 1
    for k, v in pairs(kvTable) do
        result[i] = k
        i = i + 1
    end
    return result
end

--- Use a generator/iterator to create a key/value table.
function table.generate( ... )
    local result = {}
    for key, value in ... do
        result[key] = value
    end
    return result
end

--- Use a generator/iterator to create a list.
function table.generateValues( ... )
    local result = {}
    local i = 1
    for value in ... do
        result[i] = value
        i = i + 1
    end
    return result
end

--- Generate a list like you could with a numeric for statement.
function table.generateRange( startValue, endValue, step )
    local result = {}
    local i = 1
    for value = startValue, endValue, step or 1 do
        result[i] = value
        i = i + 1
    end
    return result
end
