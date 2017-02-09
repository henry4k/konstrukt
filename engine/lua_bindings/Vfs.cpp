#include "../Common.h"
#include "../Lua.h"
#include "../Vfs.h"
#include "Vfs.h"


static int Lua_ReadFile( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    VfsFile* file = OpenVfsFile(vfsPath, VFS_OPEN_READ);
    if(!file)
    {
        luaL_error(l, "Can't open file '%s' for reading.", vfsPath);
        return 0;
    }

    const int fileSize = GetVfsFileSize(file);
    char* fileContent = (char*)malloc(fileSize);
    ReadVfsFile(file, fileContent, fileSize);
    CloseVfsFile(file);

    lua_pushlstring(l, fileContent, fileSize);
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
        luaL_error(l, "Unknown mode '%c'.", modeStr[0]);
        return 0;
    }

    VfsFile* file = OpenVfsFile(vfsPath, mode);
    if(!file)
    {
        luaL_error(l, "Can't open file '%s' for writing.", vfsPath);
        return 0;
    }

    if(content)
        WriteVfsFile(file, content, (int)contentSize);
    CloseVfsFile(file);
    return 0;
}

static int Lua_GetDirEntries( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    PathList* list = GetVfsDirEntries(vfsPath);
    if(!list)
    {
        luaL_error(l, "Can't read directory '%s'.", vfsPath);
        return 0;
    }

    lua_createtable(l, list->length, 0);

    REPEAT(list->length, i)
    {
        const Path* entry = list->data + i;
        lua_pushstring(l, entry->str);
        lua_rawseti(l, -2, i+1);
    }

    FreePathList(list);
    return 1;
}

static int Lua_GetFileInfo( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    VfsFileInfo info = GetVfsFileInfo(vfsPath);
    if(info.type == FILE_TYPE_INVALID)
        return 0;

    lua_createtable(l, 0, 1);
    switch(info.type)
    {
        case FILE_TYPE_REGULAR:
            lua_pushstring(l, "regular");
            break;

        case FILE_TYPE_DIRECTORY:
            lua_pushstring(l, "directory");
            break;

        case FILE_TYPE_UNKNOWN:
        default:
            lua_pushstring(l, "unknown");
            break;
    }
    lua_setfield(l, -2, "type");
    return 1;
}

static int Lua_DeleteFile( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    if(!DeleteVfsFile(vfsPath))
    {
        luaL_error(l, "Can't delete '%s'.", vfsPath);
        return 0;
    }
    return 0;
}

static int Lua_MakeDir( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    if(!MakeVfsDir(vfsPath))
    {
        luaL_error(l, "Can't create directory '%s'.", vfsPath);
        return 0;
    }
    return 0;
}

bool RegisterVfsInLua()
{
    return
        RegisterFunctionInLua("ReadFile", Lua_ReadFile) &&
        RegisterFunctionInLua("WriteFile", Lua_WriteFile) &&
        RegisterFunctionInLua("GetDirEntries", Lua_GetDirEntries) &&
        RegisterFunctionInLua("GetFileInfo", Lua_GetFileInfo) &&
        RegisterFunctionInLua("DeleteFile", Lua_DeleteFile) &&
        RegisterFunctionInLua("MakeDir", Lua_MakeDir);
}
