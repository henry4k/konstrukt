#ifndef __KONSTRUKT_RENDER_MANAGER__
#define __KONSTRUKT_RENDER_MANAGER__

struct JobManager;

void InitRenderManager();
void DestroyRenderManager();
double GetFrameTime();
void BeginRenderManagerUpdate( void* _context, JobManager* jobManager, double _timeDelta );
void CompleteRenderManagerUpdate( void* _context, JobManager* jobManager );

#endif
