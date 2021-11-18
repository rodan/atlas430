#!/bin/bash

. gen_functions.sh

# temporary output directory
output_dir='/tmp/2'

out_head_MSP430FR2xx_4xx()
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

out_tail_MSP430FR2xx_4xx()
{
    cat << EOF
#else
    #error "USE_XT1 was defined but pins not known in 'glue/MSP430FR2xx_4xx/clock_pin.c'"
#endif
#endif
}
EOF
}

rm -f ${output_dir}/*.c;  
bash get_specs.sh -f 'XIN' -F 'MSP430FR2xx_4xx' -s clock -d "${output_dir}"
bash get_specs.sh -f 'XOUT' -F 'MSP430FR2xx_4xx' -s clock -d "${output_dir}"

# output (generated) source file
output_file='/tmp/clock_pin.c'

for source_in in "${output_dir}"/*_clock.c; do
    source_out=$(echo "${source_in}" | sed 's|_clock.c|_clock_comb.c|')
    bitwise_comb "${source_in}" "${source_out}"
done

out_head_MSP430FR2xx_4xx > "${output_file}"
sha256sum ${output_dir}/*_clock_comb.c > "${output_dir}/clocks.hash"
ifdef_comb "${output_dir}/clocks.hash" >> "${output_file}"
out_tail_MSP430FR2xx_4xx >> "${output_file}"

##bash get_specs.sh -f 'XIN' -T 'msp430fr2.*' -s clock -d "${output_dir}"
##bash get_specs.sh -f 'XOUT' -T 'msp430fr2.*' -s clock -d "${output_dir}"

