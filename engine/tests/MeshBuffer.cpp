#include <string.h> // memset
#include <engine/MeshBuffer.h>
#include <glm/gtc/matrix_transform.hpp>
#include "TestTools.h"

using namespace glm;


Vertex* CreateVertex( float x, float y, float z )
{
    static Vertex v;
    memset(&v, 0, sizeof(v));
    v.position = vec3(x,y,z);
    v.normal = vec3(0,1,0);
    return &v;
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);

    Describe("MeshBuffer")
        .use(dummySignalSandbox)

        .it("can be created, modified and freed.", [](){

            MeshBuffer* buffer = CreateMeshBuffer();
            AddVertexToMeshBuffer(buffer, CreateVertex(0,0,0));
            AddIndexToMeshBuffer(buffer, 0);
            FreeMeshBuffer(buffer);
        })

        .it("can be transformed.", [](){

            MeshBuffer* buffer = CreateMeshBuffer();

            AddVertexToMeshBuffer(buffer, CreateVertex(0,0,0));
            AddIndexToMeshBuffer(buffer, 0);
            AddVertexToMeshBuffer(buffer, CreateVertex(0,1,0));
            AddIndexToMeshBuffer(buffer, 1);

            mat4 m(1); // identity matrix
            m = translate(m, vec3(1,0,0));
            TransformMeshBuffer(buffer, &m);

            const Vertex* vertices = GetMeshBufferVertices(buffer);
            Require(vertices[0].position == vec3(1,0,0));
            Require(vertices[1].position == vec3(1,1,0));
            // TODO: Test if normal and tangent are transformed correctly.

            FreeMeshBuffer(buffer);
        })

        .it("can be appended to another buffer.", [](){

            MeshBuffer* a = CreateMeshBuffer();

            AddVertexToMeshBuffer(a, CreateVertex(0,0,0));
            AddIndexToMeshBuffer(a, 0);
            AddVertexToMeshBuffer(a, CreateVertex(0,1,0));
            AddIndexToMeshBuffer(a, 1);

            MeshBuffer* b = CreateMeshBuffer();

            AddVertexToMeshBuffer(b, CreateVertex(2,3,4));
            AddIndexToMeshBuffer(b, 0);
            AddVertexToMeshBuffer(b, CreateVertex(5,6,7));
            AddIndexToMeshBuffer(b, 1);

            AppendMeshBuffer(a, b, NULL);

            const Vertex* vertices = GetMeshBufferVertices(a);
            const VertexIndex* indices = GetMeshBufferIndices(a);

            Require(GetMeshBufferVertexCount(a) == 4);
            Require(vertices[2].position == vec3(2,3,4));
            Require(vertices[3].position == vec3(5,6,7));
            Require(GetMeshBufferIndexCount(a) == 4);
            Require(indices[2] == 2);
            Require(indices[3] == 3);

            FreeMeshBuffer(a);
            FreeMeshBuffer(b);
        })

        .it("can be appended to another buffer while transforming it.", [](){

            MeshBuffer* a = CreateMeshBuffer();

            AddVertexToMeshBuffer(a, CreateVertex(0,0,0));
            AddIndexToMeshBuffer(a, 0);
            AddVertexToMeshBuffer(a, CreateVertex(0,1,0));
            AddIndexToMeshBuffer(a, 1);

            MeshBuffer* b = CreateMeshBuffer();

            AddVertexToMeshBuffer(b, CreateVertex(2,3,4));
            AddIndexToMeshBuffer(b, 0);
            AddVertexToMeshBuffer(b, CreateVertex(5,6,7));
            AddIndexToMeshBuffer(b, 1);

            mat4 m(1); // identity matrix
            m = translate(m, vec3(1,0,0));

            AppendMeshBuffer(a, b, &m);

            const Vertex* vertices = GetMeshBufferVertices(a);
            const VertexIndex* indices = GetMeshBufferIndices(a);

            Require(GetMeshBufferVertexCount(a) == 4);
            Require(vertices[2].position == vec3(3,3,4));
            Require(vertices[3].position == vec3(6,6,7));
            Require(GetMeshBufferIndexCount(a) == 4);
            Require(indices[2] == 2);
            Require(indices[3] == 3);
            // TODO: Test if normal and tangent are transformed correctly.

            FreeMeshBuffer(a);
            FreeMeshBuffer(b);
        });

    return RunTests();
}
