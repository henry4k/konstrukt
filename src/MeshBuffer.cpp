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

void BuildMesh( const MeshBuffer* buffer, Mesh* meshOut )
{
    const int vertexCount = buffer->vertices.size();
    const int indexCount = buffer->indices.size();

    meshOut->vertices = new Vertex[vertexCount];
    meshOut->indices  = new unsigned short[indexCount];

    meshOut->vertexCount = vertexCount;
    meshOut->indexCount  = indexCount;

    memcpy(meshOut->vertices, &buffer->vertices[0], vertexCount*sizeof(Vertex));
    memcpy(meshOut->indices, &buffer->indices[0], indexCount*sizeof(unsigned short));
}

void TransformMeshBufferRange( MeshBuffer* buffer, const glm::mat4* transformation, int firstVertex, int vertexCount )
{
    assert(firstVertex >= 0);
    assert(vertexCount >= 0);
    assert(firstVertex+vertexCount <= buffer->vertices.size());

    Vertex* vertex = &buffer->vertices[firstVertex];
    const Vertex* end = &buffer->vertices[firstVertex+vertexCount];
    for(; vertex != end; ++vertex)
    {
        glm::vec4 v(
            vertex->position.x,
            vertex->position.y,
            vertex->position.z,
            1
        );
        v = *transformation * v;
        vertex->position.x = v.x;
        vertex->position.y = v.y;
        vertex->position.z = v.z;
    }
}

void TransformMeshBuffer( MeshBuffer* buffer, const glm::mat4* transformation )
{
    TransformMeshBufferRange(buffer, transformation, 0, buffer->vertices.size());
}

void AddMeshToMeshBuffer( MeshBuffer* buffer, const Mesh* mesh, const glm::mat4* transformation )
{
    const int oldVertexCount = buffer->vertices.size();
    buffer->vertices.resize(oldVertexCount+mesh->vertexCount);
    memcpy(&buffer->vertices[oldVertexCount], mesh->vertices, mesh->vertexCount*sizeof(Vertex));

    const int oldIndexCount = buffer->indices.size();
    buffer->indices.resize(oldIndexCount+mesh->indexCount);
    memcpy(&buffer->indices[oldIndexCount], mesh->indices, mesh->indexCount*sizeof(unsigned short));

    {
        unsigned short* index = &buffer->indices[oldIndexCount];
        const unsigned short* end = &buffer->indices[oldIndexCount+mesh->indexCount];
        for(; index != end; ++index)
        {
            *index += oldVertexCount;
            assert(*index < oldVertexCount+mesh->vertexCount);
        }
    }

    if(transformation)
        TransformMeshBufferRange(buffer, transformation, oldVertexCount, mesh->vertexCount);
}

void AddMeshBufferToMeshBuffer( MeshBuffer* buffer, const MeshBuffer* otherBuffer, const glm::mat4* transformation )
{
    /* ISO C++ doesn't allow this. :(
    const Mesh mesh = {
        .vertices    = &otherBuffer->vertices[0],
        .vertexCount =  otherBuffer->vertices.size(),
        .indices     = &otherBuffer->indices[0],
        .indexCount  =  otherBuffer->indices.size()
    };
    */

    Mesh mesh;
    mesh.vertices    = const_cast<Vertex*>(&otherBuffer->vertices[0]);
    mesh.vertexCount =  otherBuffer->vertices.size();
    mesh.indices     = const_cast<unsigned short*>(&otherBuffer->indices[0]);
    mesh.indexCount  =  otherBuffer->indices.size();
    // Shame on you henry!
    // Everyone! Shame him!!

    AddMeshToMeshBuffer(buffer, &mesh, transformation);
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

SQInteger Squirrel_BuildMesh( HSQUIRRELVM vm )
{
    MeshBuffer** buffer;
    sq_getuserdata(vm, 2, (SQUserPointer*)&buffer, NULL);

    Mesh* mesh = (Mesh*)CreateUserDataInSquirrel(vm, sizeof(Mesh), OnReleaseMesh);
    BuildMesh(*buffer, mesh);
    return 1;
}
RegisterStaticFunctionInSquirrel(BuildMesh, 2, ".u");

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

SQInteger Squirrel_AddMeshToMeshBuffer( HSQUIRRELVM vm )
{
    MeshBuffer** buffer;
    sq_getuserdata(vm, 2, (SQUserPointer*)&buffer, NULL);

    Mesh* mesh;
    sq_getuserdata(vm, 3, (SQUserPointer*)&mesh, NULL);

    glm::mat4* transformation = NULL;
    if(sq_gettop(vm) > 3)
        sq_getuserdata(vm, 4, (SQUserPointer*)&transformation, NULL);

    AddMeshToMeshBuffer(*buffer, mesh, transformation);
    return 0;
}
RegisterStaticFunctionInSquirrel(AddMeshToMeshBuffer, -3, ".uuu");

SQInteger Squirrel_AddMeshBufferToMeshBuffer( HSQUIRRELVM vm )
{
    MeshBuffer** buffer;
    sq_getuserdata(vm, 2, (SQUserPointer*)&buffer, NULL);

    MeshBuffer** otherBuffer;
    sq_getuserdata(vm, 3, (SQUserPointer*)&otherBuffer, NULL);

    glm::mat4* transformation = NULL;
    if(sq_gettop(vm) > 3)
        sq_getuserdata(vm, 4, (SQUserPointer*)&transformation, NULL);

    AddMeshBufferToMeshBuffer(*buffer, *otherBuffer, transformation);
    return 0;
}
RegisterStaticFunctionInSquirrel(AddMeshBufferToMeshBuffer, -3, ".uuu");
