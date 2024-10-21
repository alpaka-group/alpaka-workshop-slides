#!/usr/bin/env bash

#set -x

base="$(echo "$1" | sed 's|/$||')"
shift
targets=("$@")

mapfile -t files < <(
    git diff --name-only -- "$base" |
        sed -E "s#(.*/|^)${base}/?##"
)
echo "Will apply diff from $base with file..."
for file in "${files[@]}"; do
    echo -e "\t... $file to:"
    diff="$(git diff -- "$base/$file")"
    # echo "$diff" | sed 's|^|\t%% |'
    for current in "${targets[@]}"; do
        current="$(echo "$current" | sed 's|/$||')"
        patched_diff="$(echo "$diff" | sed -E "s#(a|b)/.*/$base/$file#$current/$file#g")"
        echo -e "\t\t$current"
        patch --merge=diff3 -p0 "$current/$file" <(echo "$diff") | sed 's|^|\t\t\t|'
    done
done