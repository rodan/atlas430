#!/bin/bash

extract_defs()
{
    grep '^#define' "${1}" | sed 's|\s\+| |g;s|\(.*\)\s*//.*|\1|;s|\s*$||;s|#define |-D|g;s| |=|g' | grep -Ev '(_H_)|(=.*=)' | xargs
}

while (( "$#" )); do
    [ -e "${1}" ] && EXTR_STR="$(extract_defs "${1}") ${EXTR_STR}"
    shift;
done

echo "${EXTR_STR}"
