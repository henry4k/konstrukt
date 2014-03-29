#include "Common.h"
#include "Math.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Squirrel.h"
#include "MeshBuffer.h"

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
