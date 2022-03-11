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
    ucb_lc=$(echo "${ucb_id}" | tr '[:upper:]' '[:lower:]')

    cat << EOF
/*
  pin setup for the spi ${ucb_id} subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on $(date -u)
*/

#include <msp430.h>

void spi_${ucb_lc}_pin_init(void)
{

#ifdef SPI_USES_${ucb_id}

EOF
}

out_tail()
{
    family="$1"
    ucb_id="$2"
    ucb_lc=$(echo "${ucb_id}" | tr '[:upper:]' '[:lower:]')

    cat << EOF
#else
    #warning "SPI_USES_${ucb_id} was defined but pins not known in 'glue/${family}/spi_${ucb_lc}_pin.c'"
#endif

#endif
}
EOF
}

out_head_spi_md()
{
    cat << EOF

## spi pin detection map

the automated device-specific datasheet parser has detected the following pins

Legend

code | description
---- | -------
\-   | pin not available or not detected during parsing
sh   | pin belongs to the P1-11,J ports and it shares multiple functions
pm   | default port-mapped function
de   | dedicated pin
ns   | microcontroller not supported

*Table: spi pin detection map*

uc         | uca0 | uca1 | uca2 | uca3 | ucb0 | ucb1 | ucb2 | ucb3
---------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ----
EOF
}


gen_spi()
{
    family="$1"
    ucb_id="$2"
    ucb_lc=$(echo "${ucb_id}" | tr '[:upper:]' '[:lower:]')

    # output (generated) source file
    output_file="${output_dir}/${family}/spi_${ucb_lc}_pin.c"
    mkdir -p "${output_dir}/${family}"

    rm -f "${output_file}"
    bash get_specs.sh -f "${ucb_id}SOMI" -F "${family}" -s "spi_${ucb_lc}" -d "${output_dir}"
    bash get_specs.sh -f "${ucb_id}SIMO" -F "${family}" -s "spi_${ucb_lc}" -d "${output_dir}"
    bash get_specs.sh -f "${ucb_id}CLK" -F "${family}" -s "spi_${ucb_lc}" -d "${output_dir}"

    #bash get_specs.sh -f "${ucb_id}SOMI" -T 'msp430fr2153' -s "spi_${ucb_lc}" -d "${output_dir}"
    #bash get_specs.sh -f "${ucb_id}SIMO" -T 'msp430fr2153' -s "spi_${ucb_lc}" -d "${output_dir}"
    #bash get_specs.sh -f "${ucb_id}CLK" -T 'msp430fr2153' -s "spi_${ucb_lc}" -d "${output_dir}"
    #bash get_specs.sh -f "{ucb_id}SOMI" -T 'cc430f5133' -s "spi_${ucb_lc}" -d "${output_dir}"
    #bash get_specs.sh -f "{ucb_id}SIMO" -T 'cc430f5133' -s "spi_${ucb_lc}" -d "${output_dir}"
    #bash get_specs.sh -f "{ucb_id}CLK" -T 'cc430f5133' -s "spi_${ucb_lc}" -d "${output_dir}"

    for source_in in "${output_dir}"/*_spi_"${ucb_lc}".c; do
        source_out=${source_in//_spi_${ucb_lc}.c/_spi_${ucb_lc}_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head "${ucb_id}" > "${output_file}"
    sha256sum ${output_dir}/*_spi_"${ucb_lc}"_comb.c > "${output_dir}/spi_${ucb_lc}.hash"
    ifdef_comb "${output_dir}/spi_${ucb_lc}.hash" >> "${output_file}"
    out_tail "${family}" "${ucb_id}" >> "${output_file}"
}

cleanup()
{
    rm -f "${output_dir}"/*_spi_uca[0-3].c
    rm -f "${output_dir}"/*_spi_uca[0-3]_comb.c
    rm -f "${output_dir}"/spi_uca[0-3].hash
    rm -f "${output_dir}"/*_spi_ucb[0-3].c
    rm -f "${output_dir}"/*_spi_ucb[0-3]_comb.c
    rm -f "${output_dir}"/spi_ucb[0-3].hash
    sync
}

gen_code()
{
	rm -f "${output_dir}"/*_spi_uca[0-3].log
	rm -f "${output_dir}"/*_spi_ucb[0-3].log
	cleanup

	gen_spi 'MSP430FR2xx_4xx' 'UCA0'
	gen_spi 'MSP430FR2xx_4xx' 'UCA1'
	gen_spi 'MSP430FR2xx_4xx' 'UCA2'
	gen_spi 'MSP430FR2xx_4xx' 'UCA3'
	gen_spi 'MSP430FR2xx_4xx' 'UCB0'
	gen_spi 'MSP430FR2xx_4xx' 'UCB1'
	gen_spi 'MSP430FR2xx_4xx' 'UCB2'
	gen_spi 'MSP430FR2xx_4xx' 'UCB3'
	cleanup

	gen_spi 'MSP430FR5xx_6xx' 'UCA0'
	gen_spi 'MSP430FR5xx_6xx' 'UCA1'
	gen_spi 'MSP430FR5xx_6xx' 'UCA2'
	gen_spi 'MSP430FR5xx_6xx' 'UCA3'
	gen_spi 'MSP430FR5xx_6xx' 'UCB0'
	gen_spi 'MSP430FR5xx_6xx' 'UCB1'
	gen_spi 'MSP430FR5xx_6xx' 'UCB2'
	gen_spi 'MSP430FR5xx_6xx' 'UCB3'
	cleanup

	gen_spi 'MSP430F5xx_6xx' 'UCA0'
	gen_spi 'MSP430F5xx_6xx' 'UCA1'
	gen_spi 'MSP430F5xx_6xx' 'UCA2'
	gen_spi 'MSP430F5xx_6xx' 'UCA3'
	gen_spi 'MSP430F5xx_6xx' 'UCB0'
	gen_spi 'MSP430F5xx_6xx' 'UCB1'
	gen_spi 'MSP430F5xx_6xx' 'UCB2'
	gen_spi 'MSP430F5xx_6xx' 'UCB3'
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
	table="${output_dir}/spi.md"

    out_head_spi_md > "${table}"

	for uc in ${ucs}; do
		echo -n "${uc}" >> "${table}"
		for i in 0 1 2 3; do
			if [ -e "${output_dir}/${uc}_spi_ucb${i}.log" ]; then
				elem=$(get_table_elem "${uc}" "$i" < "${output_dir}/${uc}_spi_ucb${i}.log")
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

