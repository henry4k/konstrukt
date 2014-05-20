#include "../Lua.h"
#include "../MeshBuffer.h"
#include "MeshBuffer.h"

using namespace glm;


void CreateMeshBuffer( MeshBuffer* buffer )
{
    buffer->vertices.clear();
    buffer->indices.clear();
}

void FreeMeshBuffer( MeshBuffer* buffer )
{
    buffer->vertices.clear();
    buffer->indices.clear();
}

void TransformMeshBufferRange( MeshBuffer* buffer, const glm::mat4* transformation, int firstVertex, int vertexCount )
{
    using namespace glm;

    assert(firstVertex >= 0);
    assert(vertexCount >= 0);
    assert(firstVertex+vertexCount <= buffer->vertices.size());

    const mat3 rotation(*transformation);

    Vertex* vertex = &buffer->vertices[firstVertex];
    const Vertex* end = &buffer->vertices[firstVertex+vertexCount];
    for(; vertex != end; ++vertex)
    {
        vertex->position = vec3( *transformation * vec4(vertex->position, 1) );
        vertex->normal   = normalize(rotation * vertex->normal);
        vertex->tangent  = *transformation * vertex->tangent;
    }
}

void TransformMeshBuffer( MeshBuffer* buffer, const glm::mat4* transformation )
{
    TransformMeshBufferRange(buffer, transformation, 0, buffer->vertices.size());
}

void AppendMeshBuffer( MeshBuffer* buffer, const MeshBuffer* otherBuffer, const glm::mat4* transformation )
{
    buffer->indices.reserve(buffer->indices.size()+otherBuffer->indices.size());
    const VertexIndex indexOffset = buffer->indices.size();
    for(const VertexIndex& index : otherBuffer->indices)
        buffer->indices.push_back(index+indexOffset);

    const int start = buffer->vertices.size();
    buffer->vertices.insert(
        buffer->vertices.begin(),
        otherBuffer->vertices.begin(),
        otherBuffer->vertices.end()
    );
    if(transformation)
        TransformMeshBufferRange(buffer, transformation, start, otherBuffer->vertices.size());
}


// --- lua bindings ---

const char* MESH_BUFFER_TYPE = "MeshBuffer";

int Lua_MeshBuffer_destructor( lua_State* l )
{
    MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);
    FreeMeshBuffer(buffer);
    return 0;
}

int Lua_CreateMeshBuffer( lua_State* l )
{
    MeshBuffer* buffer = new MeshBuffer();
    CopyUserDataToLua(l, MESH_BUFFER_TYPE, sizeof(buffer), &buffer);
    return 1;
}

int Lua_TransformMeshBuffer( lua_State* l )
{
    MeshBuffer* buffer = CheckMeshBufferFromLua(l, 1);

    luaL_checktype(l, 2, LUA_TUSERDATA);
    const mat4* transformation = (mat4*)lua_touserdata(l, 2);

    TransformMeshBuffer(buffer, transformation);
    return 0;
}

int Lua_AppendMeshBuffer( lua_State* l )
{
    MeshBuffer* targetBuffer = CheckMeshBufferFromLua(l, 1);
    const MeshBuffer* sourceBuffer = CheckMeshBufferFromLua(l, 2);

    const mat4* transformation = NULL;
    if(lua_gettop(l) >= 3)
    {
        luaL_checktype(l, 3, LUA_TUSERDATA);
        transformation = (mat4*)lua_touserdata(l, 3);
    }

    AppendMeshBuffer(targetBuffer, sourceBuffer, transformation);
    return 0;
}

int Lua_AppendIndexToMeshBuffer( lua_State* l )
{
    MeshBuffer* targetBuffer = CheckMeshBufferFromLua(l, 1);
    const VertexIndex index = luaL_checkinteger(l, 2);
    targetBuffer->indices.push_back(index);
    return 0;
}

int Lua_AppendVertexToMeshBuffer( lua_State* l )
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

    vertex.tangent[0] = luaL_checknumber(l, i++);
    vertex.tangent[1] = luaL_checknumber(l, i++);
    vertex.tangent[2] = luaL_checknumber(l, i++);
    vertex.tangent[3] = luaL_checknumber(l, i++);

    targetBuffer->vertices.push_back(vertex);
    return 0;
}

AutoRegisterInLua()
{
    if(!RegisterUserDataTypeInLua(MESH_BUFFER_TYPE, Lua_MeshBuffer_destructor))
        return false;

    return
        RegisterFunctionInLua("CreateMeshBuffer", Lua_CreateMeshBuffer) &&
        RegisterFunctionInLua("TransformMeshBuffer", Lua_TransformMeshBuffer) &&
        RegisterFunctionInLua("AppendMeshBuffer", Lua_AppendMeshBuffer) &&
        RegisterFunctionInLua("AppendIndexToMeshBuffer", Lua_AppendIndexToMeshBuffer) &&
        RegisterFunctionInLua("AppendVertexToMeshBuffer", Lua_AppendVertexToMeshBuffer);
}

MeshBuffer* GetMeshBufferFromLua( lua_State* l, int stackPosition )
{
    return *(MeshBuffer**)GetUserDataFromLua(l, stackPosition, MESH_BUFFER_TYPE);
}

MeshBuffer* CheckMeshBufferFromLua( lua_State* l, int stackPosition )
{
    return *(MeshBuffer**)CheckUserDataFromLua(l, stackPosition, MESH_BUFFER_TYPE);
}
