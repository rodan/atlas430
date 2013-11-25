#!/bin/bash

# script that compares project sources against the reference libraries

if [ -z "${1}" ]; then
    others=`find ../ -type f -name '*.[ch]*' | grep -Ev '(reference_libs)|(examples)'`
    reference=`find ./ -type f -name '*.[ch]*' | grep -Ev '(rtca_now.h)|(sys_messagebus.h)' | sed 's|^./||'`
else
    others=`find ../ -type f -name '*.[ch]*' | grep -Ev '(reference_libs)|(examples)'`
    reference=`find ./ -type f -name '*.[ch]*' | grep -Ev '(rtca_now.h)|(sys_messagebus.h)' | grep "${1}" | sed 's|^./||'`
fi

temp_diff="/tmp/refdiff"
rm -f ${temp_diff}

echo "${others}" | while read file; do
    fpath="$(basename `dirname ${file}`)/`basename ${file}`"
    echo "${reference}" | grep -q "${fpath}" && {
        diff -up "${file}" "../reference_libs/${fpath}" >> "${temp_diff}"
    }
done

gvim "${temp_diff}"

