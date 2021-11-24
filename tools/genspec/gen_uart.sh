#!/bin/bash

#  msp430 device-specific datasheet parser 
#  collection of functions that extracts pin information from pdf datasheets
#  and generates C code used to initialize the control bits for selected pins
#  Author:          Petre Rodan <2b4eda@subdimension.ro>
#  Available from:  https://github.com/rodan/reference_libs_msp430

. gen_functions.sh

# temporary output directory
output_dir='/tmp/2'

out_head()
{
    uart_id="$1"

    cat << EOF
/*
  pin setup for UCA${uart_id} subsystem. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/reference_libs_msp430

  generated on $(date -u)
*/

#include <msp430.h>

void uart${uart_id}_pin_init(void)
{

#ifdef USE_UART${uart_id}

EOF
}

out_tail()
{
    family="$1"
    uart_id="$2"

    cat << EOF
#else
    #error "USE_UART${uart_id} was defined but pins not known in 'glue/${family}/uart${uart_id}_pin.c'"
#endif
#endif
}
EOF
}

gen_uart()
{
    family="$1"
    uart_id="$2"

    # output (generated) source file
    output_file="${output_dir}/${family}/uart${uart_id}_pin.c"
    mkdir -p "${output_dir}/${family}"

    rm -f "${output_file}"
    bash get_specs.sh -f "UCA${uart_id}RXD" -F "${family}" -s "uart${uart_id}" -d "${output_dir}"
    bash get_specs.sh -f "UCA${uart_id}TXD" -F "${family}" -s "uart${uart_id}" -d "${output_dir}"

    #bash get_specs.sh -f "UCA${uart_id}RXD" -T 'msp430bt5190' -s "uart${uart_id}" -d "${output_dir}"
    #bash get_specs.sh -f "UCA${uart_id}TXD" -T 'msp430bt5190' -s "uart${uart_id}" -d "${output_dir}"

    for source_in in "${output_dir}"/*_uart"${uart_id}".c; do
        source_out=${source_in//_uart${uart_id}.c/_uart${uart_id}_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head "${uart_id}" > "${output_file}"
    sha256sum ${output_dir}/*_uart"${uart_id}"_comb.c > "${output_dir}/uart${uart_id}.hash"
    ifdef_comb "${output_dir}/uart${uart_id}.hash" >> "${output_file}"
    out_tail "${family}" "${uart_id}" >> "${output_file}"
}

    
rm -f "${output_dir}"/*_uart[0-3].c
rm -f "${output_dir}"/*_uart[0-3]_comb.c
rm -f "${output_dir}"/uart[0-3].hash
sync

gen_uart 'MSP430FR2xx_4xx' '0'
gen_uart 'MSP430FR2xx_4xx' '1'
gen_uart 'MSP430FR5xx_6xx' '0'
gen_uart 'MSP430FR5xx_6xx' '1'
gen_uart 'MSP430FR5xx_6xx' '2'
gen_uart 'MSP430FR5xx_6xx' '3'
gen_uart 'MSP430F5xx_6xx' '0'
gen_uart 'MSP430F5xx_6xx' '1'
gen_uart 'MSP430F5xx_6xx' '2'
gen_uart 'MSP430F5xx_6xx' '3'

