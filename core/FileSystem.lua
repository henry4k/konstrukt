--- @module core.FileSystem
--- Provides file access to the mounted packages and the user directory.


local engine = require 'engine'
local Config = require 'core/Config'


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
    local metadataFileName = packageName..'/package.json'
    if FileSystem.fileExists(metadataFileName) then
        local metadata = Json.decodeFromFile(metadataFileName)
        return setmetatable(metadata, packageMetadataMetatable)
    else
        return {}
    end
end

--- Returns the metadata of a mounted package.
function FileSystem.getPackageMetadata( packageName )
    assertIsPackageName(packageName)
    local mountedPackages = FileSystem.mountedPackages
    if not mountedPackages[packageName] then
        local meta = FileSystem._loadPackageMetadata(packageName)
        mountedPackages[packageName] = meta
    end
    return mountedPackages[packageName]
end

--- Returns the contents of a regular file as a string.
-- @return[type=string]
function FileSystem.readFile( filePath )
    assertIsFilePath(filePath)
    return engine.ReadFile(filePath)
end

--- Create or replace a regular file with the given content.
-- @param filePath
-- @param[type=string] content
-- @param[type=string] mode
-- Either `w` or `a`.  Defaults to `w`.  With `a` the content is appended to
-- the file (if it exists) and `w` just replaces its content.
function FileSystem.writeFile( filePath, content, mode )
    assertIsFilePath(filePath)
    assert(type(content) == 'string', 'File content must be a string.')
    engine.WriteFile(filePath, content, mode or 'w')
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
    engine.DeleteFile(filePath)
end

--- Tests whether a file exists at the given path.
function FileSystem.fileExists( filePath )
    assertIsFilePath(filePath)
    return engine.GetFileType(filePath) ~= 'invalid'
end

--- Retrieves file type.
-- @return:
-- A table in the following entries:
--
-- - `type`: `regular`, `directory` or `unknown`
--
-- Keep in mind that the timestamps may or may not be available.
--
function FileSystem.getFileType( filePath )
    assertIsFilePath(filePath)
    local fileType = engine.GetFileType(filePath)
    if fileType == 'invalid' then
        error(string.format('\'%s\' does not exist.', filePath))
    end
    return fileType
end

--- Create a direcotry.
--
-- This is only possible in the user directory.
--
function FileSystem.makeDirectory( filePath )
    assertIsFilePath(filePath)
    engine.MakeDir(filePath)
end

local function buildEntryComparision( directory, directoriesFirst )
    local prefix
    if directory == '' then
        prefix = ''
    else
        prefix = directory..'/'
    end
    return function( a, b )
        local aIsDirectory = 0
        local bIsDirectory = 0

        local aPath = prefix..a
        local bPath = prefix..b

        if FileSystem.getFileType(aPath) == 'directory' then
            aIsDirectory = 1
        end

        if FileSystem.getFileType(bPath) == 'directory' then
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

    none = function( directory, entries )
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
-- - `none`:  Retain the file system order.
-- - `random`:  Directory entries are shuffled. (Useful for testing, see @{debug.default-sort-method})
--
function FileSystem.getDirectoryEntries( filePath, sortMethod )
    assertIsFilePath(filePath)

    sortMethod = sortMethod or defaultSortMethod
    local sortFn = sortFunctions[sortMethod]
    assert(sortFn, 'Unknown sort method: '..sortMethod)

    local entries = engine.GetDirEntries(filePath)
    sortFn(filePath, entries)
    return entries
end

--- Iterates over directory entries and provides file paths.
--
-- @param filePath
--
-- @param sortMethod
-- See @{getDirectoryEntries}.
--
-- @usage for path in FS.directory('example/directory') do ... end
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
            return entryPath
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
-- @usage for path in FS.directoryTree('example/directory') do ... end
--
function FileSystem.directoryTree( filePath, sortMethod )
    local function yieldTree( directory )
        for entryPath in FileSystem.directory(directory, sortMethod) do
            coroutine.yield(entryPath)
            if FileSystem.getFileType(entryPath) == 'directory' then
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
    elseif string.match(filePattern, '.*'..magicCharacterSet..'.*') then
        static = nil
        dynamic = filePattern
    else
        static = filePattern
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
-- @usage for path in FS.matchingFiles('example/.+%.png') do ... end
--
function FileSystem.matchingFiles( filePattern, sortMethod )
    assert(type(filePattern) == 'string', 'File pattern must be a string.')
    local staticPath = SeparateStaticAndPatternPathElements(filePattern)
    staticPath = staticPath or ''
    filePattern = '^'..filePattern..'$'
    return coroutine.wrap(function()
        for path in FileSystem.directoryTree(staticPath, sortMethod) do
            if string.match(path, filePattern) then
                coroutine.yield(path)
            end
        end
    end)
end


return FileSystem
