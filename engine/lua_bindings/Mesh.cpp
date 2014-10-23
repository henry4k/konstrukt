#include "../Lua.h"
#include "../Mesh.h"
#include "MeshBuffer.h"
#include "Mesh.h"


static int Lua_CreateMesh( lua_State* l )
{
    const MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);

    Mesh* mesh = CreateMesh(buffer);
    if(mesh)
    {
        PushPointerToLua(l, mesh);
        ReferenceMesh(mesh);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create mesh!");
        return 0;
    }
}

static int Lua_DestroyMesh( lua_State* l )
{
    Mesh* mesh = CheckMeshFromLua(l, 1);
    ReleaseMesh(mesh);
    return 0;
}

Mesh* GetMeshFromLua( lua_State* l, int stackPosition )
{
    return (Mesh*)GetPointerFromLua(l, stackPosition);
}

Mesh* CheckMeshFromLua( lua_State* l, int stackPosition )
{
    return (Mesh*)CheckPointerFromLua(l, stackPosition);
}

bool RegisterMeshInLua()
{
    return
        RegisterFunctionInLua("CreateMesh", Lua_CreateMesh);
        RegisterFunctionInLua("DestroyMesh", Lua_DestroyMesh);
}
