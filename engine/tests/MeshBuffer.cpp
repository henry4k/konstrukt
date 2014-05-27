#include <string.h> // memset
#include <engine/MeshBuffer.h>
#include <glm/gtc/matrix_transform.hpp>
#include "TestTools.h"

using namespace glm;


Vertex CreateVertex( float x, float y, float z )
{
    Vertex v;
    memset(&v, 0, sizeof(v));
    v.position = vec3(x,y,z);
    return v;
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);

    Describe("MeshBuffer")
        .use(dummySignalSandbox)

        .it("can be created, modified and freed.", [](){

            MeshBuffer buffer;
            CreateMeshBuffer(&buffer);
            buffer.vertices.push_back(CreateVertex(0,0,0));
            buffer.indices.push_back(0);
            FreeMeshBuffer(&buffer);
        })

        .it("can be transformed.", [](){

            MeshBuffer buffer;
            CreateMeshBuffer(&buffer);

            buffer.vertices.push_back(CreateVertex(0,0,0));
            buffer.indices.push_back(0);
            buffer.vertices.push_back(CreateVertex(0,1,0));
            buffer.indices.push_back(1);

            mat4 m(1); // identity matrix
            m = translate(m, vec3(1,0,0));
            TransformMeshBuffer(&buffer, &m);

            Require(buffer.vertices[0].position == vec3(1,0,0));
            Require(buffer.vertices[1].position == vec3(1,1,0));
            // TODO: Test if normal and tangent are transformed correctly.

            FreeMeshBuffer(&buffer);
        })

        .it("can be appended to another buffer.", [](){

            MeshBuffer a;
            CreateMeshBuffer(&a);

            a.vertices.push_back(CreateVertex(0,0,0));
            a.indices.push_back(0);
            a.vertices.push_back(CreateVertex(0,1,0));
            a.indices.push_back(1);

            MeshBuffer b;
            CreateMeshBuffer(&b);

            b.vertices.push_back(CreateVertex(2,3,4));
            b.indices.push_back(0);
            b.vertices.push_back(CreateVertex(5,6,7));
            b.indices.push_back(1);

            AppendMeshBuffer(&a, &b, NULL);

            Require(a.vertices.size() == 4);
            Require(a.vertices[2].position == vec3(2,3,4));
            Require(a.vertices[3].position == vec3(5,6,7));
            Require(a.indices.size() == 4);
            Require(a.indices[2] == 2);
            Require(a.indices[3] == 3);

            FreeMeshBuffer(&a);
            FreeMeshBuffer(&b);
        })

        .it("can be appended to another buffer while transforming it.", [](){

            MeshBuffer a;
            CreateMeshBuffer(&a);

            a.vertices.push_back(CreateVertex(0,0,0));
            a.indices.push_back(0);
            a.vertices.push_back(CreateVertex(0,1,0));
            a.indices.push_back(1);

            MeshBuffer b;
            CreateMeshBuffer(&b);

            b.vertices.push_back(CreateVertex(2,3,4));
            b.indices.push_back(0);
            b.vertices.push_back(CreateVertex(5,6,7));
            b.indices.push_back(1);

            mat4 m(1); // identity matrix
            m = translate(m, vec3(1,0,0));

            AppendMeshBuffer(&a, &b, &m);

            Require(a.vertices.size() == 4);
            Require(a.vertices[2].position == vec3(3,3,4));
            Require(a.vertices[3].position == vec3(6,6,7));
            Require(a.indices.size() == 4);
            Require(a.indices[2] == 2);
            Require(a.indices[3] == 3);
            // TODO: Test if normal and tangent are transformed correctly.

            FreeMeshBuffer(&a);
            FreeMeshBuffer(&b);
        });

    return RunTests();
}
