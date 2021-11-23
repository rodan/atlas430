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
    uart_name="$2"

    cat << EOF
/*
  pin setup for ${uart_name} subsystem. 
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

out_tail_MSP430FR2xx_4xx()
{
    uart_id="$1"
    uart_name="$2"

    cat << EOF
#else
    #error "USE_UART${uart_id} was defined but pins not known in 'glue/MSP430FR2xx_4xx/uart${uart_id}_pin.c'"
#endif
#endif
}
EOF
}

out_tail_MSP430FR5xx_6xx()
{
    uart_id="$1"
    uart_name="$2"

    cat << EOF
#else
    #error "USE_UART${uart_id} was defined but pins not known in 'glue/MSP430FR5xx_6xx/uart${uart_id}_pin.c'"
#endif
#endif
}
EOF
}

out_tail_MSP430F5xx_6xx()
{
    uart_id="$1"
    uart_name="$2"

    cat << EOF
#else
    #error "USE_UART${uart_id} was defined but pins not known in 'glue/MSP430FR5xx_6xx/uart${uart_id}_pin.c'"
#endif
#endif
}
EOF
}

gen_uart_MSP430FR2xx_4xx()
{
    uart_id="$1"
    uart_name="$2"

    # output (generated) source file
    output_file="/tmp/MSP430FR2xx_4xx/uart${uart_id}_pin.c"
    mkdir -p '/tmp/MSP430FR2xx_4xx'

    rm -f ${output_dir}/*.c;

    bash get_specs.sh -f "${uart_name}RXD" -F 'MSP430FR2xx_4xx' -s "uart${uart_id}" -d "${output_dir}"
    bash get_specs.sh -f "${uart_name}TXD" -F 'MSP430FR2xx_4xx' -s "uart${uart_id}" -d "${output_dir}"
    
    #bash get_specs.sh -f "${uart_name}RXD" -T 'msp430fr2512' -s "uart${uart_id}" -d "${output_dir}"
    #bash get_specs.sh -f "${uart_name}TXD" -T 'msp430fr2355' -s "uart${uart_id}" -d "${output_dir}"


    for source_in in "${output_dir}"/*_uart"${uart_id}".c; do
        source_out=${source_in//_uart${uart_id}.c/_uart${uart_id}_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head "${uart_id}" "${uart_name}" > "${output_file}"
    sha256sum ${output_dir}/*_uart"${uart_id}"_comb.c > "${output_dir}/uart${uart_id}.hash"
    ifdef_comb "${output_dir}/uart${uart_id}.hash" >> "${output_file}"
    out_tail_MSP430FR2xx_4xx "${uart_id}" "${uart_name}" >> "${output_file}"
}

gen_uart_MSP430FR5xx_6xx()
{
    uart_id="$1"
    uart_name="$2"

    # output (generated) source file
    output_file="/tmp/MSP430FR5xx_6xx/uart${uart_id}_pin.c"
    mkdir -p '/tmp/MSP430FR5xx_6xx'

    rm -f ${output_dir}/*.c;
    bash get_specs.sh -f "${uart_name}RXD" -F 'MSP430FR5xx_6xx' -s "uart${uart_id}" -d "${output_dir}"
    bash get_specs.sh -f "${uart_name}TXD" -F 'MSP430FR5xx_6xx' -s "uart${uart_id}" -d "${output_dir}"

    #bash get_specs.sh -f "${uart_name}RXD" -T 'msp430fr6041' -s "uart${uart_id}" -d "${output_dir}"
    #bash get_specs.sh -f "${uart_name}TXD" -T 'msp430fr6041' -s "uart${uart_id}" -d "${output_dir}"

    for source_in in "${output_dir}"/*_uart"${uart_id}".c; do
        source_out=${source_in//_uart${uart_id}.c/_uart${uart_id}_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head "${uart_id}" "${uart_name}" > "${output_file}"
    sha256sum ${output_dir}/*_uart"${uart_id}"_comb.c > "${output_dir}/uart${uart_id}.hash"
    ifdef_comb "${output_dir}/uart${uart_id}.hash" >> "${output_file}"
    out_tail_MSP430FR5xx_6xx "${uart_id}" "${uart_name}" >> "${output_file}"
}

gen_uart_MSP430F5xx_6xx()
{
    uart_id="$1"
    uart_name="$2"

    # output (generated) source file
    output_file="/tmp/MSP430F5xx_6xx/uart${uart_id}_pin.c"
    mkdir -p '/tmp/MSP430F5xx_6xx'

    rm -f ${output_dir}/*.c;
    #bash get_specs.sh -f "${uart_name}RXD" -F 'MSP430F5xx_6xx' -s "uart${uart_id}" -d "${output_dir}"
    #bash get_specs.sh -f "${uart_name}TXD" -F 'MSP430F5xx_6xx' -s "uart${uart_id}" -d "${output_dir}"

    bash get_specs.sh -f "${uart_name}RXD" -T 'msp430f6720' -s "uart${uart_id}" -d "${output_dir}"
    bash get_specs.sh -f "${uart_name}TXD" -T 'msp430f6720' -s "uart${uart_id}" -d "${output_dir}"

    for source_in in "${output_dir}"/*_uart"${uart_id}".c; do
        source_out=${source_in//_uart${uart_id}.c/_uart${uart_id}_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head "${uart_id}" "${uart_name}" > "${output_file}"
    sha256sum ${output_dir}/*_uart"${uart_id}"_comb.c > "${output_dir}/uart${uart_id}.hash"
    ifdef_comb "${output_dir}/uart${uart_id}.hash" >> "${output_file}"
    out_tail_MSP430F5xx_6xx "${uart_id}" "${uart_name}" >> "${output_file}"
}

#gen_uart_MSP430FR2xx_4xx '0' 'UCA0'
#gen_uart_MSP430FR2xx_4xx '1' 'UCA1'
#gen_uart_MSP430FR5xx_6xx '0' 'UCA0'
#gen_uart_MSP430FR5xx_6xx '1' 'UCA1'
#gen_uart_MSP430FR5xx_6xx '2' 'UCA2'
#gen_uart_MSP430FR5xx_6xx '3' 'UCA3'
gen_uart_MSP430F5xx_6xx '0' 'UCA0'
gen_uart_MSP430F5xx_6xx '1' 'UCA1'
gen_uart_MSP430F5xx_6xx '2' 'UCA2'
#gen_uart_MSP430F5xx_6xx '3' 'UCA3'

