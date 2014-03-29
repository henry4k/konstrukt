#!/bin/sh
Name='io_scene_json'
Version=$(cat '__init__.py' | grep '"version":' | sed 's/.*(\([0-9]*\),\([0-9]*\),\([0-9]*\).*/\1.\2.\3/')
Package="${Name}-${Version}.zip"
mkdir "$Name"
cp *.py "$Name/"
zip -r "$Package" "$Name"
rm -rf "$Name"
