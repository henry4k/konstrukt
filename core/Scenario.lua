---
-- @module core.Scenario


local FS  = require 'core/FileSystem'
local DSL = require 'core/DependencySortedList'
local ResourceManager = require 'core/ResourceManager'


local Scenario = {}

---
-- @local
-- @param packageName
-- @param dsl
function Scenario._mountPackageAndDependencies( packageName, dsl )
    local packageMetadata = FS.mountPackage(packageName)
    if packageMetadata then
        dsl:add(packageName, packageMetadata.dependencies)
        for _, dependency in ipairs(packageMetadata.dependencies) do
            if not dsl:has(dependency) then
                Scenario._mountPackageAndDependencies(dependency, dsl)
            end
        end
    else
        error('Can\'t mount package '..packageName)
    end
end

---
-- @param scenarioPackage
-- @param additionalPackages
function Scenario.load( scenarioPackage, additionalPackages )
    assert(type(scenarioPackage) == 'string', 'Scenario package must be a string')
    additionalPackages = additionalPackages or {}

    local packages = table.merge({scenarioPackage}, additionalPackages)

    -- 1. Mount initially passed packages and collect dependency information
    local dsl = DSL()
    for _, package in ipairs(packages) do
        Scenario._mountPackageAndDependencies(package, dsl)
    end

    -- 2. Work out correct load order using the dependency list
    local orderedPackages = dsl:sort()

    -- 3. Load resources and scripts of each packages using $MECHANISM
    ResourceManager.enableLoading(true)
    for _, package in ipairs(orderedPackages) do
        print('Initializing package '..package..' ..')
        for path in FS.matchingFiles(package..'/.*%.lua') do
            local moduleName = path:match('(.*)%.lua')
            require(moduleName)
        end
    end
    ResourceManager.enableLoading(false)
    print('Finished resource loading.')

    -- 4. Start scenario
    print('Starting scenario ..')
    local scenarioMetadata = FS.getPackageMetadata(scenarioPackage)
    if scenarioMetadata.type == 'scenario' then
        local mainModule = require(scenarioPackage..'/init')
        assert(mainModule.start, 'Scenarios main module must provide a start function.')
        mainModule.start()
    end
end

return Scenario
