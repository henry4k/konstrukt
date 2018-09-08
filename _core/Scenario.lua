--- @module core.Scenario
--- Loads and initializes scenarios.
--
-- See @{Packages.md}.


local FS  = require 'core/FileSystem'
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
-- 1. Execute all Lua modules, which in turn shall load all needed resources.
-- 2. Start the scenario.
--    (Call the `start` function exported by the scenarios `init` module.)
--
-- See @{Packages.md}.
--
-- @param[type=string] scenarioPackage
-- Name of the package, which shall be used as scenario.
--
function Scenario.load( scenarioPackage )
    assert(type(scenarioPackage) == 'string', 'Scenario package must be a string')

    -- 1. Execute all Lua modules, which in turn shall load all needed resources.
    print('Loading resources ...')
    ResourceManager.allowLoading(true)
    for path in FS.matchingFiles('.*%.lua') do
        if not path:match('^core/') then
            local moduleName = path:match('(.*)%.lua')
            require(moduleName)
        end
    end
    ResourceManager.allowLoading(false)
    print('Resources loaded.')
    GlobalEventSource:fireEvent('resources loaded')


    -- 2. Start scenario
    print('Starting scenario ...')
    local scenarioMetadata = FS.getPackageMetadata(scenarioPackage)
    if scenarioMetadata.type == 'scenario' then
        local mainModule = require(scenarioPackage..'/init')
        assert(mainModule.start, 'Scenarios main module must provide a start function.')
        mainModule.start()
    end
    print('Scenario started.')
    GlobalEventSource:fireEvent('scenario started')
end

return Scenario
