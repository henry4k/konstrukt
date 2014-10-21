local MountPackage        = ENGINE.MountPackage
local UnmountPackage      = ENGINE.UnmountPackage
local ReadFile            = ENGINE.ReadFile
local WriteFile           = ENGINE.WriteFile
local DeleteFile          = ENGINE.DeleteFile
local FileExists          = ENGINE.FileExists
local GetFileInfo         = ENGINE.GetFileInfo
local MakeDirectory       = ENGINE.MakeDirectory
local GetDirectoryEntries = ENGINE.GetDirectoryEntries


local FileSystem = {
    mountedPackages = {}
}

function FileSystem.loadPackageMetadata_( packageName )
    local Json = require 'core/Json'
    return Json.decodeFromFile(packageName..'/meta.json')
end

function FileSystem.mountPackage( packageName )
    if MountPackage(packageName) then
        local meta = FileSystem.loadPackageMetadata_(packageName) or {}
        FileSystem.mountedPackages[packageName] = meta
        return meta
    else
        return nil
    end
end

function FileSystem.unmountPackage( packageName )
    FileSystem.mountedPackages[packageName] = nil
    return UnmountPackage(packageName)
end

function FileSystem.getPackageMetadata( packageName )
    return FileSystem.mountedPackages[packageName]
end

function FileSystem.readFile( filePath )
    return ReadFile(filePath)
end

function FileSystem.writeFile( filePath, content )
    WriteFile(filePath, content)
end

function FileSystem.deleteFile( filePath, recursive )
    -- TODO: Implement recursive delete
    DeleteFile(filePath)
end

function FileSystem.fileExists( filePath )
    return FileExists(filePath)
end

--- Retrieves file attributes.
-- @return:
-- A table in the following entries:
-- - size: File size in bytes.
-- - mtime: File modification time as unix timestamp if available.
-- - ctime: File creation time as unix timestamp if available.
-- - type: `regular`, `directory`, `symlink` or `other`.
-- Keep in mind that the timestamps may or may not be available.
function FileSystem.getFileInfo( filePath )
    return GetFileInfo(filePath)
end

function FileSystem.makeDirectory( filePath )
    MakeDirectory(filePath)
end

function FileSystem.getDirectoryEntries( filePath )
    return GetDirectoryEntries(filePath)
end

--- Iterates over directory entries and provides file paths and file information.
-- for path, info in FS:directory('example/directory') do ... end
function FileSystem.directory( filePath )
    local entries = GetDirectoryEntries(filePath)
    local index = 0
    local count = #entries
    local prefix
    if filePath == '' then
        prefix = ''
    else
        prefix = filePath..'/'
    end
    return function()
        index = index + 1
        if index <= count then
            local entry = entries[index]
            local entryPath = prefix..entry
            local entryInfo = FileSystem.getFileInfo(entryPath)
            return entryPath, entryInfo
        end
    end
end

--- Iterates over a directory tree and provides file paths and file information.
-- for path, info in FS:directoryTree('example/directory') do ... end
function FileSystem.directoryTree( filePath )
    local function yieldTree( directory )
        for entryPath, entryInfo in FileSystem.directory(directory) do
            coroutine.yield(entryPath, entryInfo)
            if entryInfo.type == 'directory' then
                yieldTree(entryPath)
            end
        end
    end
    return coroutine.wrap(function() yieldTree(filePath) end)
end

local function SeparateStaticAndPatternPathElements( filePattern )
    local magicCharacterSet = '[%^%$%(%)%%%.%[%]%*%+%-%?]'
    local staticDynamicPattern = '([%w/]-%w)/(%w*'..magicCharacterSet..'.*)'
    local static, dynamic = string.match(filePattern, staticDynamicPattern)
    if static then
        -- do nothing
    elseif string.match(p, '.*'..magicCharacterSet..'.*') then
        static = nil
        dynamic = p
    else
        static = p
        dynamic = nil
    end
    return static, dynamic
end

--- Iterates recursivley over all files that match the given pattern.
-- for path, info in FS:matchingFiles('example/.+%.png') do ... end
function FileSystem.matchingFiles( filePattern )
    local staticPath = SeparateStaticAndPatternPathElements(filePattern)
    staticPath = staticPath or ''
    filePattern = '^'..filePattern..'$'
    return coroutine.wrap(function()
        for path, info in FileSystem.directoryTree(staticPath) do
            if string.match(path, filePattern) then
                coroutine.yield(path, info)
            end
        end
    end)
end


return FileSystem