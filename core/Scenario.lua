local FS  = require 'core/FileSystem'
local DSL = require 'core/DependencySortedList'
local ResourceManager = require 'core/ResourceManager'


local Scenario = {}

function Scenario._mountPackageAndDependencies( packageName, dsl )
    local packageInfo = FS.mountPackage(packageName)
    if packageInfo then
        local dependencies = packageInfo.dependencies or {}
        dsl:add(packageName, dependencies)
        for _, dependency in ipairs(dependencies) do
            if not dsl:has(dependency) then
                Scenario._mountPackageAndDependencies(dependency, dsl)
            end
        end
    else
        error('Can\'t mount package '..packageName)
    end
end

function Scenario.load( scenarioPackage, additionalPackages )
    assert(type(scenarioPackage) == 'string', 'Scenario package must be a string')
    additionalPackages = additionalPackages or {}

    local packages = table.merge({scenarioPackage}, additionalPackages)

    -- 1. Mount initially passed packages and collect dependecy information
    local dsl = DSL()
    for _, package in ipairs(packages) do
        Scenario._mountPackageAndDependencies(package, dsl)
    end

    -- 2. Work out correct load order using the dependency list
    local orderedPackages = dsl:sort()

    -- 3. Load resources and scripts of each packages using $MECHANISM
    ResourceManager.enableLoading(true)
    for _, package in ipairs(orderedPackages) do
        print('Loading resources of package '..package..' ..')
        -- TODO
    end
    ResourceManager.enableLoading(false)
    print('Finished resource loading.')
end

return Scenario
