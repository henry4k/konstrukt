local MountPackage        = ENGINE.MountPackage
local UnmountPackage      = ENGINE.UnmountPackage
local ReadFile            = ENGINE.ReadFile
local WriteFile           = ENGINE.WriteFile
local DeleteFile          = ENGINE.DeleteFile
local FileExists          = ENGINE.FileExists
local GetFileInfo         = ENGINE.GetFileInfo
local MakeDirectory       = ENGINE.MakeDirectory
local GetDirectoryEntries = ENGINE.GetDirectoryEntries


local FileSystem = {}

function FileSystem.mountPackage( packageName )
    return MountPackage(packageName)
end

function FileSystem.unmountPackage( packageName )
    return UnmountPackage(packageName)
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

