#include "../Lua.h"
#include "../Mesh.h"
#include "MeshBuffer.h"
#include "Mesh.h"


const char* MESH_TYPE = "Mesh";

int Lua_Mesh_destructor( lua_State* l )
{
    const Mesh* mesh = CheckMeshFromLua(l, 1);
    FreeMesh(mesh);
    return 0;
}

int Lua_CreateMesh( lua_State* l )
{
    const MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);

    Mesh* mesh = (Mesh*)PushUserDataToLua(l, MESH_TYPE, sizeof(Mesh));
    if(CreateMesh(mesh, buffer))
    {
        return 1;
    }
    else
    {
        lua_pop(l, 1);
        luaL_error(l, "Failed to create mesh!");
        return 0;
    }
}

bool RegisterMeshInLua()
{
    if(!RegisterUserDataTypeInLua(MESH_TYPE, Lua_Mesh_destructor))
        return false;

    return
        RegisterFunctionInLua("CreateMesh", Lua_CreateMesh);
}

Mesh* GetMeshFromLua( lua_State* l, int stackPosition )
{
    return (Mesh*)GetUserDataFromLua(l, stackPosition, MESH_TYPE);
}

Mesh* CheckMeshFromLua( lua_State* l, int stackPosition )
{
    return (Mesh*)CheckUserDataFromLua(l, stackPosition, MESH_TYPE);
}
