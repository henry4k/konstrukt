#ifndef __KONSTRUKT_CONFIG__
#define __KONSTRUKT_CONFIG__

void InitConfig();
void DestroyConfig();

void SetConfigString( const char* key, const char* value );
void SetConfigInt(    const char* key, int         value );
void SetConfigFloat(  const char* key, float       value );
void SetConfigBool(   const char* key, bool        value );

const char* GetConfigString( const char* key, const char* defaultValue );
int         GetConfigInt(    const char* key, int         defaultValue );
float       GetConfigFloat(  const char* key, float       defaultValue );
bool        GetConfigBool(   const char* key, bool        defaultValue );

void ReadConfigFile( const char* fileName, bool overWrite );

/**
 * Parses a config value of the form `<key>=<value>` or `<key>`.
 * The latter form will use `true` as value.
 */
void ReadConfigString( const char* str );

#endif
