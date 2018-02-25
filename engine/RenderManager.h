#ifndef __KONSTRUKT_RENDER_MANAGER__
#define __KONSTRUKT_RENDER_MANAGER__

void InitRenderManager();
void DestroyRenderManager();
double GetFrameTime();
void BeginRenderManagerUpdate( void* _context, double _timeDelta );
void CompleteRenderManagerUpdate( void* _context );

#endif
