#include "../Lua.h"
#include "../MeshBuffer.h"
#include "Math.h"
#include "MeshBuffer.h"

using namespace glm;


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
    const mat4* transformation = CheckMatrix4FromLua(l, 2);

    TransformMeshBuffer(buffer, transformation);
    return 0;
}

static int Lua_IndexMeshBuffer( lua_State* l )
{
    MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);
    IndexMeshBuffer(buffer);
    return 0;
}

static int Lua_CalcMeshBufferNormals( lua_State* l )
{
    MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);
    CalcMeshBufferNormals(buffer);
    return 0;
}

static int Lua_CalcMeshBufferTangents( lua_State* l )
{
    MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);
    CalcMeshBufferTangents(buffer);
    return 0;
}

static int Lua_AppendMeshBuffer( lua_State* l )
{
    MeshBuffer* targetBuffer = CheckMeshBufferFromLua(l, 1);
    const MeshBuffer* sourceBuffer = CheckMeshBufferFromLua(l, 2);

    const mat4* transformation = NULL;
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

    vertex.position[0] = luaL_checknumber(l, i++);
    vertex.position[1] = luaL_checknumber(l, i++);
    vertex.position[2] = luaL_checknumber(l, i++);

    vertex.color[0] = luaL_checknumber(l, i++);
    vertex.color[1] = luaL_checknumber(l, i++);
    vertex.color[2] = luaL_checknumber(l, i++);

    vertex.texCoord[0] = luaL_checknumber(l, i++);
    vertex.texCoord[1] = luaL_checknumber(l, i++);

    vertex.normal[0] = luaL_checknumber(l, i++);
    vertex.normal[1] = luaL_checknumber(l, i++);
    vertex.normal[2] = luaL_checknumber(l, i++);

    vertex.tangent   = vec3(0,0,0);
    vertex.bitangent = vec3(0,0,0);

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

bool RegisterMeshBufferInLua()
{
    return
        RegisterFunctionInLua("CreateMeshBuffer", Lua_CreateMeshBuffer) &&
        RegisterFunctionInLua("DestroyMeshBuffer", Lua_DestroyMeshBuffer) &&
        RegisterFunctionInLua("TransformMeshBuffer", Lua_TransformMeshBuffer) &&
        RegisterFunctionInLua("IndexMeshBuffer", Lua_IndexMeshBuffer) &&
        RegisterFunctionInLua("CalcMeshBufferNormals", Lua_CalcMeshBufferNormals) &&
        RegisterFunctionInLua("CalcMeshBufferTangents", Lua_CalcMeshBufferTangents) &&
        RegisterFunctionInLua("AppendMeshBuffer", Lua_AppendMeshBuffer) &&
        RegisterFunctionInLua("AppendIndexToMeshBuffer", Lua_AppendIndexToMeshBuffer) &&
        RegisterFunctionInLua("AppendVertexToMeshBuffer", Lua_AppendVertexToMeshBuffer);
}
