#!/bin/bash

#  regression test for the msp430 datasheet parser functions
#  Author:          Petre Rodan <2b4eda@subdimension.ro>
#  Available from:  https://github.com/rodan/reference_libs_msp430

. gen_functions.sh

tmp_dir='/tmp/2'

check()
{
    temp_dir="$1"

    family=$(basename "${temp_dir}")

    for c_file in "${temp_dir}"/*.c; do
        ref_file=$(basename "${c_file}")
        difference=$(diff --color --unified=0 "../../glue/${family}/${ref_file}" "${c_file}" | grep -Ev '(^\+\+\+)|(^---)|(^@@)' | grep -v 'generated on')
        if [ -n "${difference}" ]; then
            err "differences found in ${family}/${ref_file}"
            diff --color -u "../../glue/${family}/${ref_file}" "${c_file}"
        else
            inf "${family}/${ref_file} ${GOOD}Ok${NORMAL}"
        fi
    done

}

for output in "${tmp_dir}"/MSP430F*; do
    check "${output}"
done

