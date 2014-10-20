local class  = require 'middleclass'


local DSL = class('core/DependencySortedList')

function DSL:initialize()
    self.entryMap = {}
end

function DSL:has( name )
    return self.entryMap[name] ~= nil
end

function DSL:add( name, dependencies )
    assert(not self.entryMap[name], 'Entry '..name..' already exists.')
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
            return false -- dont care
        end
    end)

    local names = {}
    for i, entry in ipairs(entries) do
        names[i] = entry.name
    end
    return names
end


return DSL
