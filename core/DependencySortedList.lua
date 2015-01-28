--- A container that can be used to sort entries that have dependencies.
--
-- Each entry added to this list can 'depend' on other entries.
-- 
-- Once all entries have been added, the list can be sorted.
-- Each entry will be placed *after* all its dependencies.
--
-- This is especially useful when working out the correct loading order for
-- packages.
--
-- The list also detects dependency errors, like missing and circular
-- dependencies.
--
-- @classmod core.DependencySortedList
-- @alias DSL


local class  = require 'middleclass'


local DSL = class('core/DependencySortedList')


function DSL:initialize()
    self.entryMap = {}
end

--- Test whether the list contains an entry called `name`.
function DSL:has( name )
    return self.entryMap[name] ~= nil
end

--- Insert a new unique entry.
--
-- The same entry may not be added twice.
--
-- @param name
--
-- @param[opt] dependencies
-- A list of entry names that the added entry depends on.
--
function DSL:add( name, dependencies )
    assert(not self.entryMap[name], 'Entry '..name..' already exists.')
    dependencies = dependencies or {}
    self.entryMap[name] = dependencies
end

local function resolveTransitiveDependencies( entryMap,
                                              entryName,
                                              resolvedDependencies,
                                              dependencies )
    for _, dependency in ipairs(dependencies) do
        if not resolvedDependencies[dependency] then
            local dependencies2 = entryMap[dependency]
            if dependencies2 then
                resolvedDependencies[dependency] = true
                resolveTransitiveDependencies(entryMap,
                                              dependency,
                                              resolvedDependencies,
                                              dependencies2)
            else
                error('Detected missing dependency: '..entryName..' needs '..dependency..'.')
            end
        end
    end
end

--- Work out the correct order and return the sorted result list.
--
-- @return
-- A list which contains all entries in an order that statisfies their dependencies.
--
function DSL:sort()

    local resolvedEntryMap = {}
    for name, dependencies in pairs(self.entryMap) do
        local resolvedDependencies = {}
        resolveTransitiveDependencies(self.entryMap,
                                      name,
                                      resolvedDependencies,
                                      dependencies)
        resolvedEntryMap[name] = resolvedDependencies
    end

    local entries = {}
    for name, dependencies in pairs(resolvedEntryMap) do
        table.insert(entries, {name=name, dependencies=dependencies})
    end

    table.sort(entries, function( a, b )
        local aDependsOnB = b.dependencies[a.name]
        local bDependsOnA = a.dependencies[b.name]

        if aDependsOnB and bDependsOnA then
            error('Detected cross reference: '..a.name..' and '..b.name..' depend on each other.')
        elseif aDependsOnB then
            return true
        elseif bDependsOnA then
            return false
        else
            return false -- don't care
        end
    end)

    local names = {}
    for i, entry in ipairs(entries) do
        names[i] = entry.name
    end
    return names
end


return DSL
