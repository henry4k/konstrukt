local function GetSourcePath( stackIndex )
    local info = debug.getinfo(stackIndex+1, 'S')
    if info and
       info.source and
       info.source:sub(1,1) == '@' then
        return info.source:sub(2)
    end
end

local function GetSourceDir( stackIndex )
    local sourcePath = GetSourcePath(stackIndex+1)
    if sourcePath then
        return sourcePath:match('^(.*)/')
    end
end

-- luacheck: globals here

--- Gives the current directory or a subpath thereof.
function here( subPath )
    local path = GetSourceDir(2)
    if path then
        if subPath then
            return string.format('%s/%s', path, subPath)
        else
            return path
        end
    end
end
