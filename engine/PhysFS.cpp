#include <string.h> // strncpy
#include <physfs.h>
#include "Common.h"
#include "PhysFS.h"


static void LogPhysFSVersion()
{
    PHYSFS_Version compiled;
    PHYSFS_Version linked;

    PHYSFS_VERSION(&compiled);
    PHYSFS_getLinkedVersion(&linked);

    Log("Using PhysFS %d.%d.%d", compiled.major, compiled.minor, compiled.patch);
    Log("Linked against PhysFS %d.%d.%d", linked.major, linked.minor, linked.patch);
}

bool InitPhysFS( const int argc, char const * const * argv )
{
    LogPhysFSVersion();
    if(PHYSFS_init(argv[0]))
    {
        PHYSFS_setSaneConfig("apoapsis-team", // Organization
                             "apoapsis", // Program name
                             NULL, // Extension of automatically mounted archives
                             0, // Include CD-ROMs
                             0); // Prepend automatically found archives

        // Further initialization stuff here ...
        return true;
    }
    else
    {
        Error("%s", PHYSFS_getLastError());
        return false;
    }
}

void DestroyPhysFS()
{
    if(!PHYSFS_deinit())
        Error("%s", PHYSFS_getLastError());
}
