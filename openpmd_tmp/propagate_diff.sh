#!/usr/bin/env bash

from="$1"
shift
to="$1"
shift

base="$(find -maxdepth 1 -mindepth 1 -type d -name "${from}*" | head -n 1 | sed -E 's|^[^/]*/||')"
mapfile -t files < <(
    git diff --name-only -- "$base" |
        sed -E "s|.*/${base}/||"
)
echo "Will apply diff from $base with file..."
for file in "${files[@]}"; do
    echo -e "\t... $file to:"
    diff="$(git diff -- "$base/$file")"
    # echo "$diff" | sed 's|^|\t%% |'
    for i in $(seq -f "%02.0f" "$from" "$to" | tail -n +2); do
        # find -maxdepth 1 -mindepth -type d -name "${i}* | head -n 1
        current="$(find -maxdepth 1 -mindepth 1 -type d -name "${i}*" | head -n 1)"
        patched_diff="$(echo "$diff" | sed -E "s#(a|b)/.*/$base/$file#$current/$file#g")"
        echo -e "\t\t$current"
        patch --merge=diff3 -p0 "$current/$file" <(echo "$diff") | sed 's|^|\t\t\t|'
    done
done