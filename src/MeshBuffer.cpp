#include "Common.h"
#include "Math.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Squirrel.h"
#include "Lua.h"
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
    MeshBuffer* buffer = (MeshBuffer*)lua_touserdata(l, 1);
    FreeMeshBuffer(buffer);
    return 0;
}

int Lua_CreateMeshBuffer( lua_State* l )
{
    MeshBuffer* buffer = new MeshBuffer();
    CopyUserDataToLua(l, MESH_BUFFER_TYPE, sizeof(MeshBuffer*), buffer);
    return 1;
}

int Lua_TransformMeshBuffer( lua_State* l )
{
    MeshBuffer* buffer = GetMeshBufferFromLua(l, 1);

    luaL_checktype(l, 2, LUA_TUSERDATA);
    const mat4* transformation = (mat4*)lua_touserdata(l, 2);

    TransformMeshBuffer(buffer, transformation);
    return 0;
}

int Lua_AppendMeshBuffer( lua_State* l )
{
    MeshBuffer* targetBuffer = GetMeshBufferFromLua(l, 1);
    const MeshBuffer* sourceBuffer = GetMeshBufferFromLua(l, 2);

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
    MeshBuffer* targetBuffer = GetMeshBufferFromLua(l, 1);
    const VertexIndex index = luaL_checkinteger(l, 2);
    targetBuffer->indices.push_back(index);
    return 0;
}

int Lua_AppendVertexToMeshBuffer( lua_State* l )
{
    MeshBuffer* targetBuffer = GetMeshBufferFromLua(l, 1);

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


// --- Squirrel Bindings ---

SQInteger OnReleaseMeshBuffer( void* userData, SQInteger size )
{
    FreeMeshBuffer(*(MeshBuffer**)userData);
    return 1;
}

SQInteger Squirrel_CreateMeshBuffer( HSQUIRRELVM vm )
{
    MeshBuffer* buffer = new MeshBuffer();
    PushUserDataToSquirrel(vm, &buffer, sizeof(MeshBuffer*), OnReleaseMeshBuffer);
    return 1;
}
RegisterStaticFunctionInSquirrel(CreateMeshBuffer, 1, ".");

SQInteger Squirrel_TransformMeshBuffer( HSQUIRRELVM vm )
{
    MeshBuffer** buffer;
    sq_getuserdata(vm, 2, (SQUserPointer*)&buffer, NULL);

    glm::mat4* transformation;
    sq_getuserdata(vm, 3, (SQUserPointer*)&transformation, NULL);

    TransformMeshBuffer(*buffer, transformation);
    return 0;
}
RegisterStaticFunctionInSquirrel(TransformMeshBuffer, 3, ".uu");

SQInteger Squirrel_AppendMeshBuffer( HSQUIRRELVM vm )
{
    MeshBuffer** buffer;
    sq_getuserdata(vm, 2, (SQUserPointer*)&buffer, NULL);

    MeshBuffer** otherMeshBuffer;
    sq_getuserdata(vm, 3, (SQUserPointer*)&otherMeshBuffer, NULL);

    glm::mat4* transformation = NULL;
    if(sq_gettop(vm) > 3)
        sq_getuserdata(vm, 4, (SQUserPointer*)&transformation, NULL);

    AppendMeshBuffer(*buffer, *otherMeshBuffer, transformation);
    return 0;
}
RegisterStaticFunctionInSquirrel(AppendMeshBuffer, -3, ".uuu");

SQInteger Squirrel_AppendIndicesToMeshBuffer( HSQUIRRELVM vm )
{
    MeshBuffer** buffer = NULL;
    sq_getuserdata(vm, 2, (SQUserPointer*)&buffer, NULL);

    void* blob = NULL;
    sqstd_getblob(vm, 3, (SQUserPointer*)&blob);
    const int blobSize = sqstd_getblobsize(vm, 3);

    if(blobSize % sizeof(VertexIndex) != 0)
        return sq_throwerror(vm, "Blob size is not a multiple of the index size!");

    const int indexCount = blobSize / sizeof(VertexIndex);
    const VertexIndex* indexList = reinterpret_cast<const VertexIndex*>(blob);

    (*buffer)->indices.insert(
        (*buffer)->indices.begin(),
        &indexList[0],
        &indexList[indexCount]
    );
    return 0;
}
RegisterStaticFunctionInSquirrel(AppendIndicesToMeshBuffer, 3, ".ux");

SQInteger Squirrel_AppendVerticesToMeshBuffer( HSQUIRRELVM vm )
{
    MeshBuffer** buffer = NULL;
    sq_getuserdata(vm, 2, (SQUserPointer*)&buffer, NULL);

    void* blob = NULL;
    sqstd_getblob(vm, 3, (SQUserPointer*)&blob);
    const int blobSize = sqstd_getblobsize(vm, 3);

    if(blobSize % sizeof(Vertex) != 0)
        return sq_throwerror(vm, "Blob size is not a multiple of the vertex size!");

    const int vertexCount = blobSize / sizeof(Vertex);
    const Vertex* vertexList = reinterpret_cast<const Vertex*>(blob);

    (*buffer)->vertices.insert(
        (*buffer)->vertices.begin(),
        &vertexList[0],
        &vertexList[vertexCount]
    );
    return 0;
}
RegisterStaticFunctionInSquirrel(AppendVerticesToMeshBuffer, 3, ".ux");
