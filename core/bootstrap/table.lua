--- Extensions to the standard table functions:

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

table.copy = copy
table.include = include
table.merge = merge
table.find = find
