#ifndef __CONFIG__
#define __CONFIG__

bool InitConfig( const int argc, char** argv );
void DestroyConfig();
const char* GetConfigString( const char* key, const char* defaultValue );
int   GetConfigInt( const char* key, int defaultValue );
float GetConfigFloat( const char* key, float defaultValue );
bool  GetConfigBool( const char* key, bool defaultValue );

#endif
