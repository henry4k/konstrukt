#include <string.h>

#include "Common.h"
#include "Squirrel.h"
#include "Mesh.h"


const int MaxLineLength = 512;

struct MeshInfo
{
    FILE* file;
    char line[MaxLineLength];
    int get;
    int lineNumber;
    Mesh* mesh;
};

bool ReadLine( MeshInfo* info )
{
    FILE* file = info->file;
    char* line = info->line;
    bool eof = false;
    int i = 0;
    for(; i < MaxLineLength-1; i++) // -1 wegen '\0' am ende
    {
        int c = fgetc(file);

        if(c == EOF)
        {
            eof = true;
            break;
        }

        if(c == '\n')
        {
            break;
        }

        line[i] = c;
    }

    line[i] = '\0';
    info->lineNumber++;
    info->get = 0;
    return !eof;
}

bool CompareLazy( const char* a, const char* b )
{
    for(;;)
    {
        if((*a == '\0') || (*b == '\0'))
            break;
        if(*a != *b)
            return false;
        a++;
        b++;
    }
    return true;
}

bool LineEqual( MeshInfo* info, const char* comparedString )
{
    ReadLine(info); // ignore eof etc.
    return CompareLazy(info->line, comparedString);
}

int ReadChar( MeshInfo* info )
{
    int c = info->line[info->get];
    if(c == '\0')
        return EOF;
    info->get++;
    return c;
}

void Rewind( MeshInfo* info, int chars )
{
    info->get -= chars;
    if(info->get < 0)
        info->get = 0;
}

bool ContainsChar( const char* haystack, char needle )
{
    for(; *haystack != '\0'; haystack++)
        if(needle == *haystack)
            return true;
    return false;
}

void EatChars( MeshInfo* info, const char* eat )
{
    for(;;)
    {
        int c = ReadChar(info);
        if((c == EOF) || !ContainsChar(eat, c))
            break;
    }
    Rewind(info, 1);
}

void EatWhitespace( MeshInfo* info )
{
    EatChars(info, " \t");
}

int ReadInt( MeshInfo* info )
{
    EatWhitespace(info);
    int r = atoi(&info->line[info->get]);
    EatChars(info, "0123456789+-."); // int must also eat '.'!
    return r;
}

double ReadFloat( MeshInfo* info )
{
    EatWhitespace(info);
    double r = atof(&info->line[info->get]);
    EatChars(info, "0123456789+-.");
    return r;
}

bool ContinuesWith( MeshInfo* info, const char* text ) // eats the given text if equal
{
    const char* curLine = &info->line[info->get];
    int rewind = 0;

    for(;;)
    {
        int c = ReadChar(info);

        rewind++;

        if(c != *text)
            break;

        text++;

        if((*text == '\0') || (c == '\0'))
        {
            rewind = -1;
            break;
        }
    }
    if(rewind > 0)
        Rewind(info, rewind);

    return rewind == -1;
}

bool ReadHeader( MeshInfo* info )
{
    if(!LineEqual(info, "ply"))
    {
        Error("Incorrect magic bytes");
        return false;
    }

    if(!LineEqual(info, "format ascii 1.0"))
    {
        Error("Incorrect format");
        return false;
    }

    for(;;)
    {
        if(!ReadLine(info))
        {
            Error("Unexpected EOF");
            return false;
        }

        if(ContinuesWith(info, "element vertex "))
        {
            info->mesh->vertexCount = ReadInt(info);
            continue;
        }

        if(ContinuesWith(info, "element face "))
        {
            info->mesh->indexCount = ReadInt(info)*3;
            continue;
        }

        if(ContinuesWith(info, "end_header"))
        {
            break;
        }
    }

    return true;
}

bool ReadVertices( MeshInfo* info )
{
    Mesh* mesh = info->mesh;

    mesh->vertices = new Vertex[mesh->vertexCount];
    for(int i = 0; i < mesh->vertexCount; i++)
    {
        Vertex* vertex = &mesh->vertices[i];

        ReadLine(info);

    #define READ_VERTEX(P) vertex-> P = ReadFloat(info);
        READ_VERTEX(position.x)
        READ_VERTEX(position.y)
        READ_VERTEX(position.z)
        READ_VERTEX(normal.x)
        READ_VERTEX(normal.y)
        READ_VERTEX(normal.z)
        READ_VERTEX(texCoord.x)
        READ_VERTEX(texCoord.y)
    #undef READ_VERTEX
    }

    mesh->indices = new unsigned short[mesh->indexCount];
    for(int i = 0; i < mesh->indexCount; i += 3)
    {
        ReadLine(info);

        if(ReadInt(info) != 3)
            Error("Invalid face (only triangles are supported)");

        mesh->indices[i] = ReadInt(info);
        mesh->indices[i+1] = ReadInt(info);
        mesh->indices[i+2] = ReadInt(info);
    }

    return true;
}

struct Triangle
{
    unsigned short index[3];
};

void CalculateTangentArray( int vertexCount, Vertex* vertex, int triangleCount, const Triangle* triangle )
{
    glm::vec3* tan1 = new glm::vec3[vertexCount * 2];
    glm::vec3* tan2 = tan1 + vertexCount;
    memset(tan1, 0, sizeof(glm::vec3) * vertexCount * 2);

    for( int a = 0; a < triangleCount; a++ )
    {
        int i1 = triangle->index[0];
        int i2 = triangle->index[1];
        int i3 = triangle->index[2];

        const glm::vec3& v1 = vertex[i1].position;
        const glm::vec3& v2 = vertex[i2].position;
        const glm::vec3& v3 = vertex[i3].position;

        const glm::vec2& w1 = vertex[i1].texCoord;
        const glm::vec2& w2 = vertex[i2].texCoord;
        const glm::vec2& w3 = vertex[i3].texCoord;

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        glm::vec3 sdir;
        sdir.x = (t2 * x1 - t1 * x2) * r;
        sdir.y = (t2 * y1 - t1 * y2) * r;
        sdir.z = (t2 * z1 - t1 * z2) * r;

        glm::vec3 tdir;
        tdir.x = (s1 * x2 - s2 * x1) * r;
        tdir.y = (s1 * y2 - s2 * y1) * r;
        tdir.z = (s1 * z2 - s2 * z1) * r;

        tan1[i1] = tan1[i1] + sdir;
        tan1[i2] = tan1[i2] + sdir;
        tan1[i3] = tan1[i3] + sdir;

        tan2[i1] = tan2[i1] + tdir;
        tan2[i2] = tan2[i2] + tdir;
        tan2[i3] = tan2[i3] + tdir;

        triangle++;
    }

    for( int a = 0; a < vertexCount; a++ )
    {
        const glm::vec3& n = vertex[a].normal;
        const glm::vec3& t = tan1[a];

        // Gram-Schmidt orthogonalize
        glm::vec3* tangent3 = (glm::vec3*)&vertex[a].tangent;
        //*tangent3 = glm::normalize(t - n*glm::dot(n,t));
        *tangent3 = t - n*glm::dot(n,t);

        // Calculate handedness
        vertex[a].tangent.w = (glm::dot(glm::cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
    }

    delete[] tan1;
}

void FreeMeshInfo( const MeshInfo* info )
{
    if(info->file)
        fclose(info->file);
}

bool LoadMesh( Mesh* mesh, const char* file )
{
    FILE* f = fopen(file, "r");
    if(!f)
    {
        Error("Can't read %s", file);
        return false;
    }

    mesh->vertices = 0;
    mesh->vertexCount = 0;
    mesh->indices = 0;
    mesh->indexCount = 0;

    MeshInfo info;
    info.file = f;
    info.lineNumber = 0;
    info.mesh = mesh;

    if(!ReadHeader(&info))
    {
        Error("Parser error at %s:%d", file, info.lineNumber);
        FreeMeshInfo(&info);
        FreeMesh(mesh);
        return false;
    }

    if(!ReadVertices(&info))
    {
        Error("Parser error at %s:%d", file, info.lineNumber);
        FreeMeshInfo(&info);
        FreeMesh(mesh);
        return false;
    }

    CalculateTangentArray(mesh->vertexCount, mesh->vertices, mesh->indexCount/3, (Triangle*)mesh->indices);

    FreeMeshInfo(&info);
    Log("Loaded %s", file);
    return true;
}

void FreeMesh( const Mesh* mesh )
{
    assert(mesh->vertices);
    assert(mesh->vertexCount > 0);
    delete[] mesh->vertices;

    if(mesh->indices)
    {
        assert(mesh->indexCount > 0);
        delete[] mesh->indices;
    }
}



// --- Squirrel Bindings ---

SQInteger OnReleaseMesh( void* userData, SQInteger size )
{
    FreeMesh((Mesh*)userData);
    return 1;
}

SQInteger Squirrel_LoadMesh( HSQUIRRELVM vm )
{
    const char* fileName = NULL;
    sq_getstring(vm, 2, &fileName);

    Mesh* mesh = (Mesh*)CreateUserDataInSquirrel(vm, sizeof(Mesh), OnReleaseMesh);
    if(!LoadMesh(mesh, fileName))
    {
        sq_pop(vm, 1);
        return 0;
    }
    else
    {
        return 1;
    }
}
RegisterStaticFunctionInSquirrel(LoadMesh, 2, ".s");
