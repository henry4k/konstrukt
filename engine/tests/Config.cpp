#include <string.h> // strcmp
#include "../Config.h"
#include "TestTools.h"
#include <dummy/inline.hpp>

#define InlineTest DUMMY_INLINE_TEST


InlineTest("can be initialized empty.", dummySignalSandbox)
{
    InitConfig();
    DestroyConfig();
}

InlineTest("can set and get values.", dummySignalSandbox)
{
    InitConfig();

    SetConfigString("aaa", "bbb");
    Require(strcmp(GetConfigString("aaa",""), "bbb") == 0);

    DestroyConfig();
}

InlineTest("can convert integer values.", dummySignalSandbox)
{
    InitConfig();

    SetConfigString("aaa", "42");
    SetConfigString("bbb", "1.1");
    SetConfigString("ccc", "1.9");

    Require(GetConfigInt("aaa", 0) == 42);
    Require(GetConfigInt("bbb", 0) == 1);
    Require(GetConfigInt("ccc", 0) == 1);
    Require(GetConfigInt("xxx", 0) == 0);

    DestroyConfig();
}

InlineTest("can convert floating point values.", dummySignalSandbox)
{
    InitConfig();

    SetConfigString("aaa", "42");
    SetConfigString("bbb", "1.1");
    SetConfigString("ccc", "1.9");

    Require(GetConfigFloat("aaa", 0) == 42);
    Require(GetConfigFloat("bbb", 0) == 1.1f);
    Require(GetConfigFloat("ccc", 0) == 1.9f);
    Require(GetConfigFloat("xxx", 0.1f) == 0.1f);

    DestroyConfig();
}

InlineTest("can convert boolean values.", dummySignalSandbox)
{
    InitConfig();

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

    DestroyConfig();
}

InlineTest("can parse ini files.", dummySignalSandbox)
{
    InitConfig();

    ReadConfigFile("data/Test.ini", true);

    Require(strcmp(GetConfigString("aaa", ""), "bbb") == 0);
    Require(strcmp(GetConfigString("foo.bar", ""), "baz") == 0);
    Require(strcmp(GetConfigString("foo.hello", ""), "Hello World") == 0);

    DestroyConfig();
}

int main( int argc, char** argv )
{
    InitTests(argc, argv);
    dummyAddInlineTests();
    return RunTests();
}
