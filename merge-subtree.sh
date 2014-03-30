#!/bin/sh
if [ -z "$1" ]; then
    echo "Merges the given subtree branch into the current one." 1>&2
    echo "Usage: $0 <subtree-branch>" 1>&2
    exit 1
fi
git merge --squash -s subtree --no-commit "$1"
