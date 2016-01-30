#!/bin/bash
# vim: set filetype=sh:
cd $(dirname $0)

function Count
{
   echo $#
}

LuaFiles=$(find ../core -name '*.lua' -print)
LuaModules=$(echo $LuaFiles | sed -E 's_\.\./core/([^ ]+)\.lua_\1_g')

echo "1..$(Count $LuaModules)"

i=1
for module in $LuaModules; do
    testFile="${module}.t"
    if [ -x "$testFile" ]; then
        echo "ok $i - $module has a test"
    else
        echo "not ok $i - $module has no test"
    fi
    i=$(($i + 1))
done
