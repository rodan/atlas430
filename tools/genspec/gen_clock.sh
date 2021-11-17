#!/bin/bash

. gen_functions.sh

# temporary output directory
output_dir='/tmp/2'

# output (generated) source file
output_file='/tmp/clock_pin.c'

out_head()
{
    cat << EOF
/*
  pin setup for crystal-based clocks. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430

  generated on $(date -u)
*/

#include <msp430.h>

void clock_pin_init(void)
{
#ifdef USE_XT1

EOF
}

out_tail()
{
    cat << EOF
    #else
    #error "USE_XT1 was defined but pins not known in 'glue/MSP430FR2xx_4xx/clock.c'"
    #endif
#endif
}
EOF
}

rm ${output_dir}/*.c;  
#bash get_specs.sh -f 'XIN' -T 'msp430fr2.*' -s clock -d "${output_dir}"
#bash get_specs.sh -f 'XOUT' -T 'msp430fr2.*' -s clock -d "${output_dir}"
bash get_specs.sh -f 'XIN' -F 'MSP430FR2xx_4xx' -s clock -d "${output_dir}"
bash get_specs.sh -f 'XOUT' -F 'MSP430FR2xx_4xx' -s clock -d "${output_dir}"


for source in ${output_dir}/*_clock.c; do

    source_comb=$(echo ${source} | sed 's|_clock.c|_clock_comb.c|')
    > "${source_comb}"

    cat "${source}" | while read line; do
        if echo "${line}" | grep -q 'P[0-9JSELDIR]* &= .*'; then
            register=$(echo "${line}" | grep -o 'P[0-9JSELDIR]*')
            #search for the same register with the same operator
            register_comb=$(grep "${register} &=" "${source}")
            register_comb_lncnt=$(echo "${register_comb}" | wc -l)
            if [ "${register_comb_lncnt}" == "2" ]; then
                #extract values
                values=$(echo "${register_comb}" | awk '{ print $3 }' | sed 's|;||g' | sort -n | xargs)
                negations_cnt=$(echo "${values}" | tr -d -c '~' | wc -c)
                if [ "${negations_cnt}" == "2" ]; then
                    new_value=$(echo "${values}" | sed 's|~||g;s|[ ]| \| |')
                    new_line="    ${register} &= ~(${new_value});"
                    grep -q "^${new_line}$" "${source_comb}" || 
                        echo "${new_line}" >> "${source_comb}"
                else
                    echo "${line}" >> "${source_comb}"
                fi
                #echo ${values}
            else
                echo "${line}" >> "${source_comb}"
            fi
        elif echo "${line}" | grep -q 'P[0-9SELDIR]* |= .*'; then
            register=$(echo "${line}" | grep -o 'P[0-9JSELDIR]*')
            #search for the same register with the same operator
            register_comb=$(grep "${register} |=" "${source}")
            register_comb_lncnt=$(echo "${register_comb}" | wc -l)
            if [ "${register_comb_lncnt}" == "2" ]; then
                #extract values
                values=$(echo "${register_comb}" | awk '{ print $3 }' | sed 's|;||g' | sort -n | xargs)
                negations_cnt=$(echo "${values}" | tr -d -c '~' | wc -c)
                if [ "${negations_cnt}" == "0" ]; then
                    new_value=$(echo "${values}" | sed 's|[ ]| \| |')
                    new_line="    ${register} |= ${new_value};"
                    grep -q "^${new_line}$" "${source_comb}" || 
                        echo "${new_line}" >> "${source_comb}"
                else
                    echo "${line}" >> "${source_comb}"
                fi
                #echo ${values}
            else
                echo "${line}" >> "${source_comb}"
            fi

        else
            # line not recognized, output as is
            echo "${line}"
        fi
    done
done

out_head > "${output_file}"

sha256sum ${output_dir}/*_clock_comb.c > "${output_dir}/clocks.hash"
unique_hashes=$(awk '{ print $1 }' "${output_dir}/clocks.hash" | sort -u)

> "${output_dir}/clock_pins.c"

first_ifdef_in_file=true
for hash in ${unique_hashes}; do
    unset ifdef_line
    sources=$(grep "${hash}" "${output_dir}/clocks.hash" | awk '{ print $2 }')
    first_ifdef_in_line=true
    for source in ${sources}; do
        uc_name=$(basename "${source}" | sed 's|_clock_comb.c||')
        target=$(convert_name_to_target "${uc_name}")
        if ${first_ifdef_in_file}; then
            echo -n "    #if defined (${target})" >> "${output_file}"
            first_ifdef_in_file=false
            first_ifdef_in_line=false
        else
            if ${first_ifdef_in_line}; then
                echo -n "    #elif defined (${target})" >> "${output_file}"
                first_ifdef_in_line=false
            else 
                echo -n " || defined (${target})" >> "${output_file}"
            fi
        fi
    done
    echo '' >> "${output_file}"
    cat ${source} >> "${output_file}"
done

out_tail >> "${output_file}"

