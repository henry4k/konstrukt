#include <string.h> // memset
#include <engine/MeshBuffer.h>
#include "TestTools.h"


Vertex* CreateVertex( float x, float y, float z )
{
    static Vertex v;
    memset(&v, 0, sizeof(v));
    Vec3 position = {{x,y,z}};
    Vec3 normal   = {{0,1,0}};
    v.position = position;
    v.normal   = normal;
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

            const Vec3 v = {{1,0,0}};
            const Mat4 m = TranslateMat4(Mat4Identity, v);
            TransformMeshBuffer(buffer, m);

            const Vertex* vertices = GetMeshBufferVertices(buffer);
            const Vec3 v1 = {{1,0,0}};
            const Vec3 v2 = {{1,1,0}};
            Require(vertices[0].position == v1);
            Require(vertices[1].position == v2);
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

            const Vertex* vertices     = GetMeshBufferVertices(a);
            const VertexIndex* indices = GetMeshBufferIndices(a);

            Require(GetMeshBufferVertexCount(a) == 4);
            const Vec3 v2 = {{2,3,4}};
            const Vec3 v3 = {{5,6,7}};
            Require(vertices[2].position == v2);
            Require(vertices[3].position == v3);
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

            const Vec3 v = {{1,0,0}};
            const Mat4 m = TranslateMat4(Mat4Identity, v);

            AppendMeshBuffer(a, b, &m);

            const Vertex* vertices     = GetMeshBufferVertices(a);
            const VertexIndex* indices = GetMeshBufferIndices(a);

            Require(GetMeshBufferVertexCount(a) == 4);
            const Vec3 v2 = {{3,3,4}};
            const Vec3 v3 = {{6,6,7}};
            Require(vertices[2].position == v2);
            Require(vertices[3].position == v3);
            Require(GetMeshBufferIndexCount(a) == 4);
            Require(indices[2] == 2);
            Require(indices[3] == 3);
            // TODO: Test if normal and tangent are transformed correctly.

            FreeMeshBuffer(a);
            FreeMeshBuffer(b);
        });

    return RunTests();
}
