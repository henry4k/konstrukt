package.path = '?;'..package.path

local fileSearcher = function( name )
    local path, message = package.searchpath(name, package.path, '/')
    if path then
        local loader = function( path ) return loadfile(path)() end
        return loader, path
    else
        return message
    end
end

table.insert(package.searchers, 1, fileSearcher)

print('CORE/INIT LOADED')
