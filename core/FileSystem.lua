--- @module core.FileSystem
--- Provides file access to the mounted packages and the user directory.


local Config = require 'core/Config'
local MountPackage        = ENGINE.MountPackage
local UnmountPackage      = ENGINE.UnmountPackage
local ReadFile            = ENGINE.ReadFile
local WriteFile           = ENGINE.WriteFile
local DeleteFile          = ENGINE.DeleteFile
local FileExists          = ENGINE.FileExists
local GetFileInfo         = ENGINE.GetFileInfo
local MakeDirectory       = ENGINE.MakeDirectory
local GetDirectoryEntries = ENGINE.GetDirectoryEntries


local function assertIsPackageName( packageName )
    if not type(packageName) == 'string' then
        error('Is not a valid package name.')
    end
end

local function assertIsFilePath( filePath )
    if not type(filePath) == 'string' then
        error('Is not a valid file path.')
    end
end

--- Default values for package metadata.
-- @internal
-- @see documentation/Packages.md
local packageMetadataMetatable = {
    __index = {
        type = 'regular',
        dependencies = {}
    }
}


local FileSystem = {
    mountedPackages = {}
}

function FileSystem._loadPackageMetadata( packageName )
    assertIsPackageName(packageName)
    local Json = require 'core/Json'
    local metadata = Json.decodeFromFile(packageName..'/meta.json')
    return setmetatable(metadata, packageMetadataMetatable)
end

--- Mounts the package and returns its metadata on success.
function FileSystem.mountPackage( packageName )
    assertIsPackageName(packageName)
    if MountPackage(packageName) then
        local meta = FileSystem._loadPackageMetadata(packageName) or {}
        FileSystem.mountedPackages[packageName] = meta
        return meta
    else
        return nil
    end
end

--- Unmounts a previously mounted package.
--
-- This does *not* unload any loaded resources of that package.
--
-- @return[type=boolean] `true` if the operation succeeded.
--
function FileSystem.unmountPackage( packageName )
    assertIsPackageName(packageName)
    FileSystem.mountedPackages[packageName] = nil
    return UnmountPackage(packageName)
end

--- Returns the metadata of a mounted package.
function FileSystem.getPackageMetadata( packageName )
    assertIsPackageName(packageName)
    return FileSystem.mountedPackages[packageName]
end

--- Returns the contents of a regular file as a string.
-- @return[type=string]
function FileSystem.readFile( filePath )
    assertIsFilePath(filePath)
    return ReadFile(filePath)
end

--- Create or replace a regular file with the given content.
-- @param filePath
-- @param[type=string] content
function FileSystem.writeFile( filePath, content )
    assertIsFilePath(filePath)
    assert(type(content) == 'string', 'File content must be a string.')
    WriteFile(filePath, content)
end

--- Delete a file.
--
-- This works only for files in the user directory, not for mounted modules.
--
-- @param filePath
--
-- @param[type=boolean] recursive
-- Whether to delete directories recursively.
--
function FileSystem.deleteFile( filePath, recursive )
    assertIsFilePath(filePath)
    assert(not recursive, 'Recursive deleting is not supported yet.')
    -- TODO: Implement recursive delete
    DeleteFile(filePath)
end

--- Tests whether a file exists at the given path.
function FileSystem.fileExists( filePath )
    assertIsFilePath(filePath)
    return FileExists(filePath)
end

--- Retrieves file attributes.
-- @return:
-- A table in the following entries:
--
-- - `size`: File size in bytes.
-- - `mtime`: File modification time as unix timestamp if available.
-- - `ctime`: File creation time as unix timestamp if available.
-- - `type`: `regular`, `directory`, `symlink` or `other`.
--
-- Keep in mind that the timestamps may or may not be available.
--
function FileSystem.getFileInfo( filePath )
    assertIsFilePath(filePath)
    return GetFileInfo(filePath)
end

--- Create a direcotry.
--
-- This is only possible in the user directory.
--
function FileSystem.makeDirectory( filePath )
    assertIsFilePath(filePath)
    MakeDirectory(filePath)
end

local function buildEntryComparision( directory, directoriesFirst )
    local prefix = directory..'/'
    return function( a, b )
        local aIsDirectory = 0
        local bIsDirectory = 0

        local aPath = prefix..a
        local bPath = prefix..b

        if FileSystem.getFileInfo(aPath).type == 'directory' then
            aIsDirectory = 1
        end

        if FileSystem.getFileInfo(bPath).type == 'directory' then
            bIsDirectory = 1
        end

        if aIsDirectory ~= bIsDirectory then
            if directoriesFirst then
                return aIsDirectory > bIsDirectory
            else
                return aIsDirectory < bIsDirectory
            end
        end

        return a < b
    end
end

local sortFunctions = {
    alphabetic = function( directory, entries )
        table.sort(entries)
    end,

    directoriesFirst = function( directory, entries )
        table.sort(entries, buildEntryComparision(directory, true))
    end,

    directoriesLast = function( directory, entries )
        table.sort(entries, buildEntryComparision(directory, false))
    end,

    fileSystem = function( directory, entries )
        -- Just retain the file system order.
    end,

    random = function( directory, entries )
        table.shuffle(entries)
    end
}

--- Determines which sort method is used by default.
-- @configkey debug.default-sort-method
-- Defaults to `directoriesLast`.
local defaultSortMethod = Config.get('debug.default-sort-method', 'directoriesLast')
assert(sortFunctions[defaultSortMethod], 'Unknown default sort method: '..defaultSortMethod)

--- Retrieves contents of a directory.
--
-- @param filePath
--
-- @param[type=string] sortMethod
-- Optional, see `defaultSortMethod`.
--
-- - `alphabetic`:  Order entries alphabetically.
-- - `directoriesFirst`:  Like `alphabetic`, but prefer directories.
-- - `directoriesLast`:  Like `alphabetic`, but defer directories.
-- - `fileSystem`:  Retain the file system order.
-- - `random`:  Directory entries are shuffled. (Useful for testing, see @{debug.default-sort-method})
--
function FileSystem.getDirectoryEntries( filePath, sortMethod )
    assertIsFilePath(filePath)

    sortMethod = sortMethod or defaultSortMethod
    local sortFn = sortFunctions[sortMethod]
    assert(sortFn, 'Unknown sort method: '..sortMethod)

    local entries = GetDirectoryEntries(filePath)
    sortFn(filePath, entries)
    return entries
end

--- Iterates over directory entries and provides file paths and file information.
--
-- @param filePath
--
-- @param sortMethod
-- See @{getDirectoryEntries}.
--
-- @usage for path, info in FS.directory('example/directory') do ... end
--
function FileSystem.directory( filePath, sortMethod )
    local entries = FileSystem.getDirectoryEntries(filePath, sortMethod)
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
--
-- @param filePath
--
-- @param sortMethod
-- See @{getDirectoryEntries}.
--
-- @usage for path, info in FS.directoryTree('example/directory') do ... end
--
function FileSystem.directoryTree( filePath, sortMethod )
    local function yieldTree( directory )
        for entryPath, entryInfo in FileSystem.directory(directory, sortMethod) do
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
--
-- @param filePattern
--
-- @param sortMethod
-- See @{getDirectoryEntries}.
--
-- @usage for path, info in FS.matchingFiles('example/.+%.png') do ... end
--
function FileSystem.matchingFiles( filePattern, sortMethod )
    assert(type(filePattern) == 'string', 'File pattern must be a string.')
    local staticPath = SeparateStaticAndPatternPathElements(filePattern)
    staticPath = staticPath or ''
    filePattern = '^'..filePattern..'$'
    return coroutine.wrap(function()
        for path, info in FileSystem.directoryTree(staticPath, sortMethod) do
            if string.match(path, filePattern) then
                coroutine.yield(path, info)
            end
        end
    end)
end


return FileSystem
