#ifndef __KONSTRUKT_MESH__
#define __KONSTRUKT_MESH__

struct MeshBuffer;
struct Mesh;


Mesh* CreateMesh( const MeshBuffer* buffer );
void DrawMesh( const Mesh* mesh );

void ReferenceMesh( Mesh* mesh );
void ReleaseMesh( Mesh* mesh );

#endif
