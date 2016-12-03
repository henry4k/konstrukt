#include "../Common.h"
#include "../Lua.h"
#include "../PhysFS.h"
#include "PhysFS.h"


static int Lua_MountPackage( lua_State* l )
{
    const char* reference = luaL_checkstring(l, 1);
    const bool result = MountPackage(reference);
    lua_pushboolean(l, result);
    return 1;
}

static int Lua_ReadFile( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    PHYSFS_File* file = PHYSFS_openRead(vfsPath);
    if(!file)
    {
        luaL_error(l, "Can't open file '%s' for reading: %s",
                      vfsPath, PHYSFS_getLastError());
        return 0;
    }

    const int size = (int)PHYSFS_fileLength(file);

    char* buffer = new char[size];
    const int bytesRead = PHYSFS_readBytes(file, buffer, size);
    PHYSFS_close(file);

    if(bytesRead == size)
    {
        lua_pushlstring(l, buffer, size);
        return 1;
    }
    else
    {
        delete[] buffer;
        luaL_error(l, "Can't read file '%s': %s", vfsPath, PHYSFS_getLastError());
        return 0;
    }
}

static int Lua_WriteFile( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    luaL_checktype(l, 2, LUA_TSTRING);
    size_t size = 0;
    const char* content = lua_tolstring(l, 2, &size);


    PHYSFS_File* file = PHYSFS_openWrite(vfsPath);
    if(!file)
    {
        luaL_error(l, "Can't open file '%s' for writing: %s",
                      vfsPath, PHYSFS_getLastError());
        return 0;
    }

    const bool success = PHYSFS_writeBytes(file, content, size);
    PHYSFS_close(file);

    if(!success)
        luaL_error(l, "Can't write file '%s': %s", vfsPath, PHYSFS_getLastError());
    return 0;
}

static int Lua_DeleteFile( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    if(!PHYSFS_delete(vfsPath))
        luaL_error(l, "Can't delete '%s': %s", vfsPath, PHYSFS_getLastError());
    return 0;
}

static int Lua_FileExists( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    lua_pushboolean(l, PHYSFS_exists(vfsPath));
    return 1;
}

static int Lua_GetFileInfo( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);

#if defined(PHYSFS_NO_STAT)
    if(PHYSFS_exists(vfsPath))
    {
        lua_createtable(l, 0, 3); // 3 entries

        PHYSFS_File* file = PHYSFS_openRead(vfsPath);
        if(file)
        {
            lua_pushinteger(l, PHYSFS_fileLength(file));
            lua_setfield(l, -2, "size");
            PHYSFS_close(file);
        }

        const PHYSFS_sint64 mtime = PHYSFS_getLastModTime(vfsPath);
        if(mtime >= 0)
        {
            lua_pushinteger(l, mtime);
            lua_setfield(l, -2, "mtime");
        }

        if(PHYSFS_isDirectory(vfsPath))
            lua_pushstring(l, "directory");
        else
            lua_pushstring(l, "regular");
        lua_setfield(l, -2, "type");

        return 1;
    }
    else
    {
        luaL_error(l, "Can't stat '%s'.", vfsPath);
        return 0;
    }
#else
    PHYSFS_Stat statResult;
    if(PHYSFS_stat(vfsPath, &statResult))
    {
        lua_createtable(l, 0, 5); // 5 entries

        if(statResult.filesize >= 0)
        {
            lua_pushinteger(l, statResult.filesize);
            lua_setfield(l, -2, "size");
        }

        if(statResult.modtime >= 0)
        {
            lua_pushinteger(l, statResult.modtime);
            lua_setfield(l, -2, "mtime");
        }

        if(statResult.createtime >= 0)
        {
            lua_pushinteger(l, statResult.createtime);
            lua_setfield(l, -2, "ctime");
        }

        // atime is intentially not provided, since not all file systems
        // support it and archives won't provide correct values anyway.

        switch(statResult.filetype)
        {
            case PHYSFS_FILETYPE_REGULAR:
                lua_pushstring(l, "regular");
                break;

            case PHYSFS_FILETYPE_DIRECTORY:
                lua_pushstring(l, "directory");
                break;

            case PHYSFS_FILETYPE_SYMLINK:
                lua_pushstring(l, "symlink");
                break;

            case PHYSFS_FILETYPE_OTHER:
                lua_pushstring(l, "other");
                break;
        }
        lua_setfield(l, -2, "type");

        return 1;
    }
    else
    {
        luaL_error(l, "Can't stat '%s': %s", vfsPath, PHYSFS_getLastError());
        return 0;
    }
#endif
}

static int Lua_MakeDirectory( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    if(!PHYSFS_mkdir(vfsPath))
        luaL_error(l, "Can't create directory '%s': %s", vfsPath, PHYSFS_getLastError());
    return 0;
}

struct EnumerateFilesContext
{
    lua_State* luaState;
    int currentIndex;
};

static void EnumerateFilesCallback( void* context_,
                                    const char* directory,
                                    const char* entryName )
{
    EnumerateFilesContext* context = (EnumerateFilesContext*)context_;
    lua_State* l = context->luaState;
    lua_pushstring(l, entryName);
    lua_rawseti(l, -2, context->currentIndex);
    context->currentIndex++;
}

static int Lua_GetDirectoryEntries( lua_State* l )
{
    const char* vfsPath = luaL_checkstring(l, 1);
    lua_createtable(l, 0, 0);
    EnumerateFilesContext context = { l, 1 };
    PHYSFS_enumerateFilesCallback(vfsPath, EnumerateFilesCallback, &context);
    return 1;
}

bool RegisterPhysFSInLua()
{
    return
        RegisterFunctionInLua("MountPackage", Lua_MountPackage) &&
        RegisterFunctionInLua("ReadFile", Lua_ReadFile) &&
        RegisterFunctionInLua("WriteFile", Lua_WriteFile) &&
        RegisterFunctionInLua("DeleteFile", Lua_DeleteFile) &&
        RegisterFunctionInLua("FileExists", Lua_FileExists) &&
        RegisterFunctionInLua("GetFileInfo", Lua_GetFileInfo) &&
        RegisterFunctionInLua("MakeDirectory", Lua_MakeDirectory) &&
        RegisterFunctionInLua("GetDirectoryEntries", Lua_GetDirectoryEntries);
}
