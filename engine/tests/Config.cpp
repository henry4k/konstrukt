#include <string.h> // strcmp
#include "../Config.h"
#include "TestTools.h"
#include <dummy/bdd.hpp>

using namespace dummy;


class ConfigScope
{
private:
    bool destroyed;

public:
    ConfigScope() { InitConfig(); destroyed = false; }
    ~ConfigScope() { if(!destroyed) DestroyConfig(); }
    void destroy() { DestroyConfig(); destroyed = true; };
};

int main( int argc, char** argv )
{
    InitTests(argc, argv);

    Describe("Config module")
        .use(dummySignalSandbox)

        .it("can be initialized empty.", [](){

            InitConfig();
            DestroyConfig();
        })

        .it("can set and get values.", [](){

            ConfigScope scope;
            SetConfigString("aaa", "bbb");
            Require(strcmp(GetConfigString("aaa",""), "bbb") == 0);
        })

        .it("can convert integer values.", [](){

            ConfigScope scope;
            SetConfigString("aaa", "42");
            SetConfigString("bbb", "1.1");
            SetConfigString("ccc", "1.9");

            Require(GetConfigInt("aaa", 0) == 42);
            Require(GetConfigInt("bbb", 0) == 1);
            Require(GetConfigInt("ccc", 0) == 1);
            Require(GetConfigInt("xxx", 0) == 0);
        })

        .it("can convert floating point values.", [](){

            ConfigScope scope;
            SetConfigString("aaa", "42");
            SetConfigString("bbb", "1.1");
            SetConfigString("ccc", "1.9");

            Require(GetConfigFloat("aaa", 0) == 42);
            Require(GetConfigFloat("bbb", 0) == 1.1f);
            Require(GetConfigFloat("ccc", 0) == 1.9f);
            Require(GetConfigFloat("xxx", 0.1f) == 0.1f);
        })

        .it("can convert boolean values.", [](){

            ConfigScope scope;
            SetConfigString("aaa", "42");
            SetConfigString("bbb", "1.1");
            SetConfigString("ccc", "1.9");
            SetConfigString("ddd", "0");
            SetConfigString("eee", "0.0");
            SetConfigString("fff", "true");
            SetConfigString("ggg", "false");
            SetConfigString("hhh", "yes");
            SetConfigString("jjj", "no");

            Require(GetConfigBool("aaa", false) == true);
            Require(GetConfigBool("bbb", false) == true);
            Require(GetConfigBool("ccc", false) == true);
            Require(GetConfigBool("ddd", true) == false);
            Require(GetConfigBool("eee", true) == false);
            Require(GetConfigBool("fff", false) == true);
            Require(GetConfigBool("ggg", true) == false);
            Require(GetConfigBool("hhh", false) == true);
            Require(GetConfigBool("jjj", true) == false);

            Require(GetConfigBool("xxx", true) == true);
            Require(GetConfigBool("xxx", false) == false);
        })

        .it("can parse ini files.", [](){

            ConfigScope scope;
            ReadConfigFile("data/Test.ini");

            Require(strcmp(GetConfigString("aaa", ""), "bbb") == 0);
            Require(strcmp(GetConfigString("foo.bar", ""), "baz") == 0);
            Require(strcmp(GetConfigString("foo.hello", ""), "Hello World") == 0);
        });

    return RunTests();
}
