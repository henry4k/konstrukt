#include <map>
#include <string>
#include <ini.h>

#include "Common.h"
#include "PhysFS.h"
#include "Config.h"


using namespace std;


static map<string,string> g_ConfigValues;


static void SetConfigValue( const char* key, const char* value )
{
    Log("%s = %s", key, value);
    g_ConfigValues[key] = value;
}

static int IniEntryCallback( void* user, const char* section, const char* name, const char* value );

static void ParseIniFile( const char* fileName )
{
    Log("Reading config file %s ..", fileName);
    ini_parse(fileName, IniEntryCallback, NULL);
}

static void ParseArguments( const int argc, char const * const * argv )
{
    // Parse command line arguments ...
    for(int i = 1; i < argc; ++i)
    {
        const string arg = argv[i];
        // --key=value

        if(arg.size() < 2 ||
           arg[0] != '-'  ||
           arg[1] != '-')
            break;

        const size_t equalsPos = arg.find('=');

        if(equalsPos == string::npos)
            break;

        string key   = arg.substr(2, equalsPos-2);
        string value = arg.substr(equalsPos+1);

        if(key == "config")
            ParseIniFile(value.c_str());
        else
            SetConfigValue(key.c_str(), value.c_str());
    }
}

bool InitConfig( const int argc, char const * const * argv )
{
    const char* userConfig = Format("%s/config.ini", GetUserDataDirectory());
    ParseIniFile(userConfig);

    ParseArguments(argc, argv);

    return true;
}

void DestroyConfig()
{
    g_ConfigValues.clear();
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

    SetConfigValue(key.c_str(), value);
    return 1;
}
