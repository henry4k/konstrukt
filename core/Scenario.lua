local FS = require 'core/FileSystem'


local Scneario = {}

function Scenario.mountPackageAndDependencies_( packageName )
    local packageInfo = FS.mountPackage(packageName)
    if packageInfo then
        local dependencies = packageInfo.dependencies or {}
        for _, dependency in ipairs(dependencies) do

        end
    else
        error('Can\'t mount package '..packageName)
    end
end

function Scneario.load( scenarioPackage, additionalPackages )
    -- 2. Collect dependencies recursively (and mount those packages)
    -- 3. Work out correct load order using the depency list (Depdency Sorted List)
    -- 4. Load resources and scripts of each packages using $MECHANISM
    -- 5. Lock resource loading

    local packages = table.merge({scenarioPackage}, additionalPackages)

    -- 1. Mount initially passed packages
    for _, package in ipairs() do
    end



    -- FS.getPackageMetadata()
end

return Scneario
