#include "../Common.h"
#include "../Lua.h"
#include "../Vfs.h"
#include "Vfs.h"


static int Lua_ReadFile( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    VfsFile* file = OpenVfsFile(vfsPath, VFS_OPEN_READ);

    const int fileSize = GetVfsFileSize(file);
    char* fileContent = (char*)Alloc(fileSize);
    ReadVfsFile(file, fileContent, fileSize);
    CloseVfsFile(file);

    lua_pushlstring(l, fileContent, fileSize);
    Free(fileContent);
    return 1;
}

static int Lua_WriteFile( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    size_t contentSize;
    const char* content = lua_tolstring(l, 2, &contentSize);
    const char* modeStr = luaL_checkstring(l, 3);

    VfsOpenMode mode;
    if(modeStr[0] == 'w')
        mode = VFS_OPEN_WRITE;
    else if(modeStr[0] == 'a')
        mode = VFS_OPEN_APPEND;
    else
    {
        FatalError("Unknown mode '%c'.", modeStr[0]);
        mode = VFS_OPEN_WRITE;
    }

    VfsFile* file = OpenVfsFile(vfsPath, mode);

    if(content)
        WriteVfsFile(file, content, (int)contentSize);
    CloseVfsFile(file);
    return 0;
}

static int Lua_GetDirEntries( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    PathList list = GetVfsDirEntries(vfsPath);

    lua_createtable(l, list.length, 0);

    REPEAT(list.length, i)
    {
        const Path* entry = list.data + i;
        lua_pushstring(l, entry->str);
        lua_rawseti(l, -2, i+1);
    }

    DestroyPathList(&list);
    return 1;
}

static int Lua_GetFileType( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    switch(GetVfsFileType(vfsPath))
    {
        case FILE_TYPE_REGULAR:
            lua_pushstring(l, "regular");
            break;

        case FILE_TYPE_DIRECTORY:
            lua_pushstring(l, "directory");
            break;

        case FILE_TYPE_UNKNOWN:
            lua_pushstring(l, "unknown");
            break;

        case FILE_TYPE_INVALID:
            lua_pushstring(l, "invalid");
            break;
    }
    return 1;
}

static int Lua_DeleteFile( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    DeleteVfsFile(vfsPath);
    return 0;
}

static int Lua_MakeDir( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    MakeVfsDir(vfsPath);
    return 0;
}

void RegisterVfsInLua()
{
    RegisterFunctionInLua("ReadFile", Lua_ReadFile);
    RegisterFunctionInLua("WriteFile", Lua_WriteFile);
    RegisterFunctionInLua("GetDirEntries", Lua_GetDirEntries);
    RegisterFunctionInLua("GetFileType", Lua_GetFileType);
    RegisterFunctionInLua("DeleteFile", Lua_DeleteFile);
    RegisterFunctionInLua("MakeDir", Lua_MakeDir);
}
