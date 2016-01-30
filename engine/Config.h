#ifndef __KONSTRUKT_CONFIG__
#define __KONSTRUKT_CONFIG__

bool InitConfig( const int argc, char const * const * argv );
void DestroyConfig();
const char* GetConfigString( const char* key, const char* defaultValue );
int   GetConfigInt( const char* key, int defaultValue );
float GetConfigFloat( const char* key, float defaultValue );
bool  GetConfigBool( const char* key, bool defaultValue );

#endif
