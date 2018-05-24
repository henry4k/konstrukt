#include "../Lua.h"
#include "../MeshBuffer.h"
#include "Math.h"
#include "JobManager.h"
#include "MeshBuffer.h"


static int Lua_CreateMeshBuffer( lua_State* l )
{
    MeshBuffer* buffer = CreateMeshBuffer();
    if(buffer)
    {
        PushPointerToLua(l, buffer);
        ReferenceMeshBuffer(buffer);
        return 1;
    }
    else
    {
        luaL_error(l, "Failed to create mesh buffer!");
        return 0;
    }
}

static int Lua_DestroyMeshBuffer( lua_State* l )
{
    MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);
    ReleaseMeshBuffer(buffer);
    return 0;
}

static int Lua_TransformMeshBuffer( lua_State* l )
{
    MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);
    const Mat4* transformation = CheckMatrix4FromLua(l, 2);

    TransformMeshBuffer(buffer, *transformation);
    return 0;
}

static int Lua_BeginMeshBufferPostprocessing( lua_State* l )
{
    MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);

    static const char* optionNames[] =
    {
        "indices",
        "normals",
        "tangents",
        NULL
    };

    static int optionMap[] =
    {
        MESH_BUFFER_INDEX,
        MESH_BUFFER_CALC_NORMALS,
        MESH_BUFFER_CALC_TANGENTS
    };

    int options = 0;
    const int top = lua_gettop(l);
    for(int i = 2; i < top; i++)
    {
        const int index = luaL_checkoption(l, i, NULL, optionNames);
        options |= optionMap[index];
    }

    PushJobToLua(l, BeginMeshBufferPostprocessing(buffer, options));
    return 1;
}

static int Lua_AppendMeshBuffer( lua_State* l )
{
    MeshBuffer* targetBuffer = CheckMeshBufferFromLua(l, 1);
    const MeshBuffer* sourceBuffer = CheckMeshBufferFromLua(l, 2);

    const Mat4* transformation = NULL;
    if(lua_gettop(l) >= 3)
        transformation = CheckMatrix4FromLua(l, 3);

    AppendMeshBuffer(targetBuffer, sourceBuffer, transformation);
    return 0;
}

static int Lua_AppendIndexToMeshBuffer( lua_State* l )
{
    MeshBuffer* targetBuffer = CheckMeshBufferFromLua(l, 1);
    const VertexIndex index = luaL_checkinteger(l, 2);
    AddIndexToMeshBuffer(targetBuffer, index);
    return 0;
}

static int Lua_AppendVertexToMeshBuffer( lua_State* l )
{
    MeshBuffer* targetBuffer = CheckMeshBufferFromLua(l, 1);

    int i = 2;
    Vertex vertex;

    vertex.position._[0] = luaL_checknumber(l, i++);
    vertex.position._[1] = luaL_checknumber(l, i++);
    vertex.position._[2] = luaL_checknumber(l, i++);

    vertex.color._[0] = luaL_checknumber(l, i++);
    vertex.color._[1] = luaL_checknumber(l, i++);
    vertex.color._[2] = luaL_checknumber(l, i++);

    vertex.texCoord._[0] = luaL_checknumber(l, i++);
    vertex.texCoord._[1] = luaL_checknumber(l, i++);

    vertex.normal._[0] = luaL_checknumber(l, i++);
    vertex.normal._[1] = luaL_checknumber(l, i++);
    vertex.normal._[2] = luaL_checknumber(l, i++);

    vertex.tangent   = Vec3Zero;
    vertex.bitangent = Vec3Zero;

    AddVertexToMeshBuffer(targetBuffer, &vertex);
    return 0;
}

MeshBuffer* GetMeshBufferFromLua( lua_State* l, int stackPosition )
{
    return (MeshBuffer*)GetPointerFromLua(l, stackPosition);
}

MeshBuffer* CheckMeshBufferFromLua( lua_State* l, int stackPosition )
{
    return (MeshBuffer*)CheckPointerFromLua(l, stackPosition);
}

void RegisterMeshBufferInLua()
{
    RegisterFunctionInLua("CreateMeshBuffer", Lua_CreateMeshBuffer);
    RegisterFunctionInLua("DestroyMeshBuffer", Lua_DestroyMeshBuffer);
    RegisterFunctionInLua("TransformMeshBuffer", Lua_TransformMeshBuffer);
    RegisterFunctionInLua("BeginMeshBufferPostprocessing", Lua_BeginMeshBufferPostprocessing);
    RegisterFunctionInLua("AppendMeshBuffer", Lua_AppendMeshBuffer);
    RegisterFunctionInLua("AppendIndexToMeshBuffer", Lua_AppendIndexToMeshBuffer);
    RegisterFunctionInLua("AppendVertexToMeshBuffer", Lua_AppendVertexToMeshBuffer);
}
