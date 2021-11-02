#!/bin/bash

config_file="${1}"

[ ! -e "${config_file}" ] && {
    echo "config file not found, exiting"
    exit 1
}

# sed options, in order:
#  transform multiple spaces into one
#  ignore C commentss identified by '//'
#  ignore ending spaces

grep '^#define' config.h | sed 's|\s\+| |g;s|\(.*\)\s*//.*|\1|;s|\s*$||;s|#define |-D|g;s| |=|g' | grep -Ev '(_H_)|(=.*=)' | xargs



