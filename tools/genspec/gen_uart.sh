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
    uca_id="$1"
    uca_lc=$(echo "${uca_id}" | tr '[:upper:]' '[:lower:]')

    cat << EOF
/*
  pin setup for uart ${uca_id} subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

  generated on $(date -u)
*/

#include <msp430.h>

void uart_${uca_lc}_pin_init(void)
{

#ifdef UART_USES_${uca_id}

EOF
}

out_tail()
{
    family="$1"
    uca_id="$2"
    uca_lc=$(echo "${uca_id}" | tr '[:upper:]' '[:lower:]')

    cat << EOF
#else
    #warning "UART_USES_${uca_id} was defined but pins not known in 'glue/${family}/uart_${uca_lc}_pin.c'"
#endif
#endif
}
EOF
}

out_head_uart_md()
{
    cat << EOF

## UART pin detection map

the automated device-specific datasheet parser has detected the following pins

Legend

code | description
---- | -------
\-   | pin not available or not detected during parsing
sh   | pin belongs to the P1-11,J ports and it shares multiple functions
pm   | default port-mapped function
de   | dedicated pin
ns   | microcontroller not supported

*Table: UART pin detection map*

uc         | uart_A0 | uart_A1 | uart_A2 | uart_A3
---------- | ----- | ----- | ----- | -----
EOF
}


gen_uart()
{
    family="$1"
    uca_id="$2"
    uca_lc=$(echo "${uca_id}" | tr '[:upper:]' '[:lower:]')

    # output (generated) source file
    output_file="${output_dir}/${family}/uart_${uca_lc}_pin.c"
    mkdir -p "${output_dir}/${family}"

    rm -f "${output_file}"
    bash get_specs.sh -f "${uca_id}RXD" -F "${family}" -s "uart_${uca_lc}" -d "${output_dir}"
    bash get_specs.sh -f "${uca_id}TXD" -F "${family}" -s "uart_${uca_lc}" -d "${output_dir}"

    #bash get_specs.sh -f "${uca_id}RXD" -T 'msp430f67671' -s "uart_${uca_lc}" -d "${output_dir}"
    #bash get_specs.sh -f "${uca_id}TXD" -T 'msp430f67671' -s "uart_${uca_lc}" -d "${output_dir}"
    #bash get_specs.sh -f "${uca_id}RXD" -T 'msp430fr2633' -s "uart_${uca_lc}" -d "${output_dir}"
    #bash get_specs.sh -f "${uca_id}TXD" -T 'msp430fr2633' -s "uart_${uca_lc}" -d "${output_dir}"

    for source_in in "${output_dir}"/*_uart_"${uca_lc}".c; do
        source_out=${source_in//_uart_${uca_lc}.c/_uart_${uca_lc}_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head "${uca_id}" > "${output_file}"
    sha256sum ${output_dir}/*_uart_"${uca_lc}"_comb.c > "${output_dir}/uart_${uca_lc}.hash"
    ifdef_comb "${output_dir}/uart_${uca_lc}.hash" >> "${output_file}"
    out_tail "${family}" "${uca_id}" >> "${output_file}"
}

cleanup()
{
    rm -f "${output_dir}"/*_uart_uca[0-3].c
    rm -f "${output_dir}"/*_uart_uca[0-3]_comb.c
    rm -f "${output_dir}"/uart_uca[0-3].hash
    sync
}

gen_code()
{
	rm -f "${output_dir}"/*_uart_uca[0-3].log
	cleanup

	gen_uart 'MSP430FR2xx_4xx' 'UCA0'
	gen_uart 'MSP430FR2xx_4xx' 'UCA1'
	gen_uart 'MSP430FR2xx_4xx' 'UCA2'
	gen_uart 'MSP430FR2xx_4xx' 'UCA3'
	cleanup

	gen_uart 'MSP430FR5xx_6xx' 'UCA0'
	gen_uart 'MSP430FR5xx_6xx' 'UCA1'
	gen_uart 'MSP430FR5xx_6xx' 'UCA2'
	gen_uart 'MSP430FR5xx_6xx' 'UCA3'
	cleanup

	gen_uart 'MSP430F5xx_6xx' 'UCA0'
	gen_uart 'MSP430F5xx_6xx' 'UCA1'
	gen_uart 'MSP430F5xx_6xx' 'UCA2'
	gen_uart 'MSP430F5xx_6xx' 'UCA3'
	cleanup
}

get_table_elem()
{
	uc="$1"
	elem="$2"
	ret="-"

	case "${elem}" in
		0)
			ret=$(grep 'UCA0.XD pin_type' | awk '{print $NF}' | sort -u)
			;;
		1)
			ret=$(grep 'UCA1.XD pin_type' | awk '{print $NF}' | sort -u)
			;;
		2)
			ret=$(grep 'UCA2.XD pin_type' | awk '{print $NF}' | sort -u)
			;;
		3)
			ret=$(grep 'UCA3.XD pin_type' | awk '{print $NF}' | sort -u)
			;;
	esac

    [ -z "${ret}" ] && ret='-'

	echo "${ret}"
}

gen_table()
{
    ucs="$(list_ucs)"
	table="${output_dir}/uart.md"

    out_head_uart_md > "${output_dir}/uart.md"

	for uc in ${ucs}; do
		echo -n "${uc}" >> "${table}"
		for i in 0 1 2 3; do
			if [ -e "${output_dir}/${uc}_uart_uca${i}.log" ]; then
				elem=$(get_table_elem "${uc}" "$i" < "${output_dir}/${uc}_uart_uca${i}.log")
				echo -n " | ${elem}" >> "${table}"
			else
				echo -n ' | -' >> "${table}"
			fi
		done
		echo '' >> "${table}"
	done
}

gen_code
gen_table

