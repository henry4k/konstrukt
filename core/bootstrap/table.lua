--- Extensions to the standard @{table} functions.
-- @script core.bootstrap.table

--- Copy entries of a set of tables into the target table.
--
-- The targets metatable is not modified.
-- Remember that this is just a shallow copy.
--
-- @return
-- The target table.
local function include( target, ... )
    for _,includedTable in ipairs({...}) do
        for key, value in pairs(includedTable) do
            target[key] = value
        end
    end
    return target
end

--- Duplicates the target table and returns the copy.
-- The copy has the same metatable like the target.
local function copy( target )
    local newTable = setmetatable({}, getmetatable(target))
    return include(newTable, target)
end

local function merge( ... )
    return include({}, ...)
end

--- Searches `value` in `target` and returns its key or nil if nothing was found.
local function find( target, value )
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
local function sortByWeight( target, weightingFn )
    local entriesWithWeighting = {}
    for i, entry in ipairs(target) do
        local weight = weightingFn(entry)
        entriesWithWeighting[i] = { entry=entry, weight=weight }
    end

    table.sort(entriesWithWeighting, weightComparision)

    for i, v in ipairs(entriesWithWeighting) do
        target[i] = v.entry
    end
end

local random = math.random

local function randomWeight( entry )
    return random()
end

--- Order the array part of a table randomly.
local function shuffle( target )
    return sortByWeight(target, randomWeight)
end

table.copy = copy
table.include = include
table.merge = merge
table.find = find
table.sortByWeight = sortByWeight
table.shuffle = shuffle
