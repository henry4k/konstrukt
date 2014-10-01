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

function FileSystem.getFileInfo( filePath )
    return GetFileInfo(filePath)
end

function FileSystem.makeDirectory( filePath )
    MakeDirectory(filePath)
end

function FileSystem.getDirectoryEntries( filePath )
    return GetDirectoryEntries(filePath)
end


return FileSystem

