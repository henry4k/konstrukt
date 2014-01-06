/**
 * Core is loaded before any user or map specific scripts.
 */

// --- Utils ---

function ConcatArrays( ... )
{
    local a = []
    foreach(v in vargv)
        a.extend(v)
    return a
}


// --- module system ---

Modules <- {}
RealRoot <- getroottable()

ModuleSearchPaths <- ["Scripts/Modules"]

function _fileExists( fileName )
{
    local file = ::RealRoot.native.io.file(fileName, "rb")
    if(file)
    {
        file.close()
        return true
    }
    else
    {
        return false
    }
}

function _tryLoadModule( moduleName )
{
    local modules = ::RealRoot.Modules
    local originalRoot = ::getroottable()

    if(moduleName in modules)
    {
        return modules[moduleName]._module.export
    }
    else
    {
        foreach(searchPath in ::ModuleSearchPaths)
        {
            local fileName = searchPath+"/"+moduleName+".nut"
            if(!_fileExists(fileName))
                continue

            local moduleRoot = {
                _module = {
                    name = moduleName,
                    file = fileName,
                    export = null
                }
            }.setdelegate(::RealRoot)

            ::setroottable(moduleRoot)
            try
            {
                local closure = ::RealRoot.native.io.loadfile(fileName, true)
                moduleRoot._module.export = closure.call(moduleRoot)
            }
            catch( e )
            {
                ::setroottable(originalRoot)
                ::error("Exception while trying to load module '"+moduleName+"' from '"+fileName+"':")
                ::error(e)
                continue
            }
            ::setroottable(originalRoot)

            if(moduleRoot._module.export == null)
                throw "Module '"+moduleName+"' exports nothing!"

            modules[moduleName] <- moduleRoot
            return moduleRoot._module.export
        }

        return null
    }
}

function require( moduleName )
{
    local moduleExport = _tryLoadModule(moduleName)
    if(moduleExport != null)
        return moduleExport
    else
        throw "Can't load module '"+moduleName+"'"
}


// --- init random ---

local time = require("system").time
local srand = require("math").srand
local config = require("config")

local randomSeed = config.GetConfigValue("core.random-seed", time()) 
print("Random seed = "+randomSeed)
srand(randomSeed)


// --- experimental stuff ---
/*
function ParentDirectory( path )
{
    local i = path.len()-1
    
    if(path[i] == '/')
        --i
    
    for(; i > 0; --i)
        if(path[i] == '/')
            return path.split(0,i-1)

    return ""
}
*/
