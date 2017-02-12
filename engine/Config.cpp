#include <map>
#include <string>
#include <stdlib.h> // atoi, atof
#include <ini.h>

#include "Common.h"
#include "Config.h"


using namespace std;


static map<string,string> g_ConfigValues;


void InitConfig()
{
}

void DestroyConfig()
{
    g_ConfigValues.clear();
}

void SetConfigString( const char* key, const char* value )
{
    g_ConfigValues[key] = value;
}

void SetConfigInt( const char* key, int value )
{
    SetConfigString(key, Format("%d", value));
}

void SetConfigFloat( const char* key, float value )
{
    SetConfigString(key, Format("%f", value));
}

void SetConfigBool( const char* key, bool value )
{
    SetConfigString(key, value ? "true" : "false");
}

const char* GetConfigString( const char* key, const char* defaultValue )
{
    const map<string,string>::const_iterator i =
        g_ConfigValues.find(key);

    if(i != g_ConfigValues.end())
        return i->second.c_str();
    else
        return defaultValue;
}

int GetConfigInt( const char* key, int defaultValue )
{
    const char* str = GetConfigString(key, NULL);
    return str==NULL ? defaultValue : atoi(str);
}

float GetConfigFloat( const char* key, float defaultValue )
{
    const char* str = GetConfigString(key, NULL);
    return str==NULL ? defaultValue : atof(str);
}

bool GetConfigBool( const char* key, bool defaultValue )
{
    const char* str = GetConfigString(key, NULL);
    if(str == NULL)
        return defaultValue;

    switch(str[0])
    {
        case '0':
        case 'n':
        case 'N':
        case 'f':
        case 'F':
            return false;

        /*
        case '1':
        case 'y':
        case 'Y':
        case 't':
        case 'T':
            return true;
        */

        default:
            return true;
    }
}

static int IniEntryCallback( void* user, const char* section, const char* name, const char* value )
{
    using namespace std;

    string key;
    if(section == NULL || section[0] == '\0')
        key = string(name);
    else
        key = string(section) + string(".") + string(name);

    SetConfigString(key.c_str(), value);
    return 1;
}

void ReadConfigFile( const char* fileName )
{
    LogInfo("Reading config file %s ..", fileName);
    ini_parse(fileName, IniEntryCallback, NULL);
}
