#!/bin/bash

cd "$(dirname "$0")"

function Test()
{
    local name=$1

    local out="$(./$name 2>/dev/null | diff -u - $name.expected)"
    if [[ -z "$out" ]]; then
        echo "ok $name"
    else
        while read line; do
            echo "# $line"
        done <<< "$out"
        echo "not ok $name"
    fi
}

echo '1..3'
Test "core"
Test "cpp"
Test "bdd"
