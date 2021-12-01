#!/bin/bash

#  msp430 device-specific datasheet parser 
#  collection of functions that extracts pin information from pdf datasheets
#  and generates C code used to initialize the control bits for selected pins
#  Author:          Petre Rodan <2b4eda@subdimension.ro>
#  Available from:  https://github.com/rodan/atlas430

. gen_functions.sh

# temporary output directory
output_dir='/tmp/2'

out_head()
{
    ucb_id="$1"

    cat << EOF
/*
  pin setup for the i2c UCB${ucb_id} subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on $(date -u)
*/

#include <msp430.h>

void i2c_ucb${ucb_id}_pin_init(void)
{

#ifdef USE_I2C_UCB${ucb_id}

EOF
}

out_tail()
{
    family="$1"
    ucb_id="$2"

    cat << EOF
#else
    #error "USE_I2C_UCB${ucb_id} was defined but pins not known in 'glue/${family}/i2c_ucb${ucb_id}_pin.c'"
#endif
#endif
}
EOF
}

out_head_i2c_md()
{
    cat << EOF

## I2C pin detection map

the automated device-specific datasheet parser has detected the following pins

Legend

code | description
---- | -------
\-   | pin not available or not detected during parsing
sh   | pin belongs to the P1-11,J ports and it shares multiple functions
pm   | default port-mapped function
de   | dedicated pin
ns   | microcontroller not supported

*Table: I2C pin detection map*

uc         | ucb0 | ucb1 | ucb2 | ucb3
---------- | ---- | ---- | ---- | ----
EOF
}


gen_i2c()
{
    family="$1"
    ucb_id="$2"

    # output (generated) source file
    output_file="${output_dir}/${family}/i2c_ucb${ucb_id}_pin.c"
    mkdir -p "${output_dir}/${family}"

    rm -f "${output_file}"
    bash get_specs.sh -f "UCB${ucb_id}SDA" -F "${family}" -s "i2c_ucb${ucb_id}" -d "${output_dir}"
    bash get_specs.sh -f "UCB${ucb_id}SCL" -F "${family}" -s "i2c_ucb${ucb_id}" -d "${output_dir}"

    #bash get_specs.sh -f "UCB${ucb_id}SDA" -T 'msp430fr6005' -s "i2c_ucb${ucb_id}" -d "${output_dir}"
    #bash get_specs.sh -f "UCB${ucb_id}SCL" -T 'msp430fr6005' -s "i2c_ucb${ucb_id}" -d "${output_dir}"
    #bash get_specs.sh -f "UCB${ucb_id}SDA" -T 'cc430f5133' -s "i2c_ucb${ucb_id}" -d "${output_dir}"
    #bash get_specs.sh -f "UCB${ucb_id}SCL" -T 'cc430f5133' -s "i2c_ucb${ucb_id}" -d "${output_dir}"

    for source_in in "${output_dir}"/*_i2c_ucb"${ucb_id}".c; do
        source_out=${source_in//_i2c_ucb${ucb_id}.c/_i2c_ucb${ucb_id}_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head "${ucb_id}" > "${output_file}"
    sha256sum ${output_dir}/*_i2c_ucb"${ucb_id}"_comb.c > "${output_dir}/i2c_ucb${ucb_id}.hash"
    ifdef_comb "${output_dir}/i2c_ucb${ucb_id}.hash" >> "${output_file}"
    out_tail "${family}" "${ucb_id}" >> "${output_file}"
}

cleanup()
{
    rm -f "${output_dir}"/*_i2c_ucb[0-3].c
    rm -f "${output_dir}"/*_i2c_ucb[0-3]_comb.c
    rm -f "${output_dir}"/i2c_ucb[0-3].hash
    sync
}

gen_code()
{
	rm -f "${output_dir}"/*_i2c_ucb[0-3].log
	cleanup

	gen_i2c 'MSP430FR2xx_4xx' '0'
	gen_i2c 'MSP430FR2xx_4xx' '1'
	cleanup

	gen_i2c 'MSP430FR5xx_6xx' '0'
	gen_i2c 'MSP430FR5xx_6xx' '1'
	gen_i2c 'MSP430FR5xx_6xx' '2'
	gen_i2c 'MSP430FR5xx_6xx' '3'
	cleanup

	gen_i2c 'MSP430F5xx_6xx' '0'
	gen_i2c 'MSP430F5xx_6xx' '1'
	gen_i2c 'MSP430F5xx_6xx' '2'
	gen_i2c 'MSP430F5xx_6xx' '3'
	cleanup
}

get_table_elem()
{
	uc="$1"
	elem="$2"
	ret="-"

	case "${elem}" in
		0)
			ret=$(grep 'UCB0SDA pin_type' | awk '{print $NF}')
			;;
		1)
			ret=$(grep 'UCB1SDA pin_type' | awk '{print $NF}')
			;;
		2)
			ret=$(grep 'UCB2SDA pin_type' | awk '{print $NF}')
			;;
		3)
			ret=$(grep 'UCB3SDA pin_type' | awk '{print $NF}')
			;;
	esac

    [ -z "${ret}" ] && ret='-'

	echo "${ret}"
}

gen_table()
{
    ucs="$(list_ucs)"
	table="${output_dir}/i2c.md"

    out_head_i2c_md > "${table}"

	for uc in ${ucs}; do
		echo -n "${uc}" >> "${table}"
		for i in 0 1 2 3; do
			if [ -e "${output_dir}/${uc}_i2c_ucb${i}.log" ]; then
				elem=$(get_table_elem "${uc}" "$i" < "${output_dir}/${uc}_i2c_ucb${i}.log")
				echo -n " | ${elem}" >> "${table}"
			else
				echo -n ' | -' >> "${table}"
			fi
		done
		echo '' >> "${table}"
	done
}

gen_code
#gen_table

# without USE_PARALLEL
# 534.40user 69.58system 8:25.61elapsed 119%CPU (0avgtext+0avgdata 15352maxresident)k
# 0inputs+776120outputs (789major+115578338minor)pagefaults 0swaps

# with USE_PARALLEL="true"
# 665.91user 88.19system 1:17.63elapsed 971%CPU (0avgtext+0avgdata 17808maxresident)k
# 0inputs+1855328outputs (12747major+100234489minor)pagefaults 0swaps

