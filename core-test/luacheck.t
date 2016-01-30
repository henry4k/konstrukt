#!/bin/sh
cd "$(dirname "$0")/.."
luacheck --config .luacheckrc --formatter TAP core
