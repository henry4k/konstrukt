local modules = {
    loaded = {},
    searchPaths = {
        '%s.lua',
        'core/third-party/%s.lua',
    }
}

function require( moduleName )
    local loadedModule = modules.loaded[moduleName]
    if loadedModule then
        return loadedModule
    else
        for _,searchPath in ipairs(modules.searchPaths) do
            local path = string.format(searchPath, moduleName)
            if ENGINE.FileExists(path) then
                local module = dofile(path)
                modules.loaded[moduleName] = module
                return module
            end
        end
        error('Can\'t find module '..moduleName..'!')
    end
end
