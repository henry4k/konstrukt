--- @module core.Scenario
--- Loads and initializes scenarios.
--
-- See @{Packages.md}.


local FS  = require 'core/FileSystem'
local DSL = require 'core/DependencySortedList'
local ResourceManager = require 'core/ResourceManager'
local GlobalEventSource = require 'core/GlobalEventSource'


local Scenario = {}

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

--- Performs the neccessary steps to load and start a scenario.
--
-- 1. Mount the required packages.
-- 2. Load resources in correct order.
-- 3. Start the scenario.
--    (Call the `start` function exported by the scenarios `init` module.)
--
-- See @{Packages.md}.
--
-- @param[type=string] scenarioPackage
-- Name of the package, which shall be loaded as scenario.
--
-- @param[opt,type=table] additionalPackages
-- List of additional packages, that shall be loaded.
--
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

    -- 3. Load resources and scripts of each package using $MECHANISM
    ResourceManager.allowLoading(true)
    for _, package in ipairs(orderedPackages) do
        print('Initializing package '..package..' ..')
        for path in FS.matchingFiles(package..'/.*%.lua') do
            local moduleName = path:match('(.*)%.lua')
            require(moduleName)
        end
    end
    ResourceManager.allowLoading(false)
    print('Finished resource loading.')
    GlobalEventSource:fireEvent('resources loaded')


    -- 4. Start scenario
    print('Starting scenario ..')
    local scenarioMetadata = FS.getPackageMetadata(scenarioPackage)
    if scenarioMetadata.type == 'scenario' then
        local mainModule = require(scenarioPackage..'/init')
        assert(mainModule.start, 'Scenarios main module must provide a start function.')
        mainModule.start()
    end
    GlobalEventSource:fireEvent('scenario started')
end

return Scenario
