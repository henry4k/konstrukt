#include <string.h> // strcmp
#include <engine/Config.h>
#include "../stubs/Common_stub.h"
#include "../TestTools.h"

class ConfigScope
{
private:
    bool destroyed;

public:
    ConfigScope() { destroyed = false; }
    ~ConfigScope() { if(!destroyed) DestroyConfig(); }
    void destroy() { DestroyConfig(); destroyed = true; };
};

int main( int argc, char** argv )
{
    InitTests(argc, argv);

    Describe("Config module")
        .use(dummyExceptionSandbox)

        .it("can be initialized empty.", [](){

            Require(InitConfig(0, NULL) == true);
            DestroyConfig();
        })

        .it("has a proper destructor.", [](){

            const char* argv[] = {"", "--aaa=bbb"};

            {
                Require(InitConfig(2, argv) == true);
                ConfigScope scope;
                Require(strcmp(GetConfigString("aaa",""), "bbb") == 0);
            }

            {
                Require(InitConfig(0, NULL) == true);
                ConfigScope scope;
                Require(GetConfigString("aaa",NULL) == NULL);
            }
        })

        .it("can parse arguments.", [](){

            const char* argv[] = {"", "--aaa=bbb", "--foo=bar", "--ccc=ddd"};

            Require(InitConfig(4, argv) == true);
            ConfigScope scope;

            Require(strcmp(GetConfigString("foo",""), "bar") == 0);
            Require(strcmp(GetConfigString("foo","yyy"), "bar") == 0);
            Require(strcmp(GetConfigString("foo",NULL), "bar") == 0);
            Require(strcmp(GetConfigString("xxx",""), "") == 0);
            Require(strcmp(GetConfigString("xxx","yyy"), "yyy") == 0);
            Require(GetConfigString("xxx",NULL) == NULL);
        })

        .it("can convert integer values.", [](){

            const char* argv[] = {"", "--aaa=42", "--bbb=1.1", "--ccc=1.9"};

            Require(InitConfig(4, argv) == true);
            ConfigScope scope;

            Require(GetConfigInt("aaa", 0) == 42);
            Require(GetConfigInt("bbb", 0) == 1);
            Require(GetConfigInt("ccc", 0) == 1);
            Require(GetConfigInt("xxx", 0) == 0);
        })

        .it("can convert floating point values.", [](){

            const char* argv[] = {"", "--aaa=42", "--bbb=1.1", "--ccc=1.9"};

            Require(InitConfig(4, argv) == true);
            ConfigScope scope;

            Require(GetConfigFloat("aaa", 0) == 42);
            Require(GetConfigFloat("bbb", 0) == 1.1f);
            Require(GetConfigFloat("ccc", 0) == 1.9f);
            Require(GetConfigFloat("xxx", 0.1f) == 0.1f);
        })

        .it("can convert boolean values.", [](){

            const char* argv[] = {
                "",
                "--aaa=42",
                "--bbb=1.1",
                "--ccc=1.9",
                "--ddd=0",
                "--eee=0.0",
                "--fff=true",
                "--ggg=false",
                "--hhh=yes",
                "--jjj=no"};

            Require(InitConfig(10, argv) == true);
            ConfigScope scope;

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

            const char* argv[] = {"", "--config=config/Test.ini"};

            Require(InitConfig(2, argv) == true);
            ConfigScope scope;

            Require(GetConfigString("config", NULL) == NULL);
            Require(strcmp(GetConfigString("aaa", ""), "bbb") == 0);
            Require(strcmp(GetConfigString("foo.bar", ""), "baz") == 0);
            Require(strcmp(GetConfigString("foo.hello", ""), "Hello World") == 0);
        });

    return RunTests();
}
