#ifndef __KONSTRUKT_LIST__
#define __KONSTRUKT_LIST__

struct List;


List* CreateList( int elementSize );
void FreeList( List* list );
int GetListLength( const List* list );
void* GetListEntry( List* list, int position );
const void* GetConstListEntry( const List* list, int position );
void* InsertListEntries( List* list, int position, int count, const void* elements );
void RemoveListEntries( List* list, int position, int count );

// Utils:
void* AppendListEntries( List* list, int count, const void* elements );
void* AppendListToList( List* destination, const List* source );


#endif
