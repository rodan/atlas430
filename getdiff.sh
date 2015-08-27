#!/bin/bash

# script that compares project sources against the reference libraries

myhome="$(basename `pwd`)"

usage() 
{
cat << EOF
    Usage: $0 [] [-r NAME] [-d NAME] [-h]
       -r NAME   filters for NAME in the reference libraries
       -d NAME   filters for NAME in the external code
EOF
exit
}

while (( "$#" )); do
	if [ "$1" = "-r" ]; then
        echo "filter for '${2}' in reference"
        reference=`find ./ -type f -name '*.[ch]*' | grep -Ev '(rtca_now.h)|(sys_messagebus.h)' | grep "${2}" | sed 's|^./||'`
        [ -z "${reference}" ] && exit 0
		shift; shift;
	elif [ "$1" = "-d" ]; then
        echo "filter for '${2}' in external code"
        others=`find ../ -type f -name '*.[ch]*' | grep -Ev "(${myhome})|(examples)" | grep "${2}"`
        [ -z "${others}" ] && exit 0
		shift; shift;
    else 
		shift;
		usage
    fi
done

def_others=`find ../ -type f -name '*.[ch]*' | grep -Ev "(${myhome})|(examples)"`
def_reference=`find ./ -type f -name '*.[ch]*' | grep -Ev '(rtca_now.h)|(sys_messagebus.h)' | sed 's|^./||'`

others=${others:-${def_others}}
reference=${reference:-${def_reference}}

temp_diff="/tmp/refdiff"
rm -f ${temp_diff}

echo "${others}" | while read file; do
    fpath="$(basename `dirname ${file}`)/`basename ${file}`"
    echo "${reference}" | grep -q "${fpath}" && {
        diff -up --ignore-all-space "${file}" "../${myhome}/${fpath}" >> "${temp_diff}"
    }
done

gvim "${temp_diff}"

