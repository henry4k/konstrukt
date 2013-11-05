/**
 * Core is loaded before any user or map specific scripts.
 */


// --- module system ---

Modules <- {}
RealRoot <- getroottable()

function require( moduleName )
{
	local modules = ::RealRoot.Modules

	if(moduleName in modules)
	{
		return modules[moduleName].exports
	}
	else
	{
		print("Loading module "+moduleName)

		local fileName = "Scripts/Modules/"+moduleName+".nut"
		
		local oldRoot = getroottable()
		local moduleRoot = {}.setdelegate(::RealRoot)

		setroottable(moduleRoot)
			local closure = ::RealRoot.native.io.loadfile(fileName, true)
			local moduleExports = closure.call(moduleRoot)
		setroottable(oldRoot)

		/*
		foreach(k,v in moduleExports)
			if(type(v) == "functions")
				moduleExports[k] = v.bindenv(moduleRoot).setroot(moduleRoot)
		*/

		modules[moduleName] <- {
			"root": moduleRoot,
			"exports": moduleExports
		}

		return moduleExports
	}
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
