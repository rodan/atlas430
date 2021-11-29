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
    cat << EOF
/*
  pin setup for crystal-based clocks. 
  this file is generated automatically based on the device datasheets
  Author:          Petre Rodan <2b4eda@subdimension.ro>
  Available from:  https://github.com/rodan/atlas430

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

out_xt1_xt2_MSP430FR5xx_6xx()
{
    cat << EOF
#else
    #error "USE_XT1 was defined but pins not known in 'glue/MSP430FR5xx_6xx/clock_pin.c'"
#endif
#endif

#ifdef USE_XT2
EOF
}

out_tail_MSP430FR5xx_6xx()
{
    cat << EOF
#else
    #error "USE_XT2 was defined but pins not known in 'glue/MSP430FR5xx_6xx/clock_pin.c'"
#endif
#endif
}
EOF
}

out_xt1_xt2_MSP430F5xx_6xx()
{
    cat << EOF
#else
    #error "USE_XT1 was defined but pins not known in 'glue/MSP430F5xx_6xx/clock_pin.c'"
#endif
#endif

#ifdef USE_XT2
EOF
}

out_tail_MSP430F5xx_6xx()
{
    cat << EOF
#else
    #error "USE_XT2 was defined but pins not known in 'glue/MSP430F5xx_6xx/clock_pin.c'"
#endif
#endif
}
EOF
}

out_head_clock_md()
{
    cat << EOF

## pin detection map

the automated device-specific datasheet parser has detected the following pins

Legend

code | description
---- | -------
NA   | pin not available or not detected during parsing
SH   | pin is part of the P1-11,J ports and it shares multiple functions  
PM   | primary port-mapped function
DE   | dedicated pin

*Table: pin detection map*

uc         | xin | xout | xt2in | xt2out
---------- | --- | ---  | ---   | ---
EOF
}

gen_clock_MSP430FR2xx_4xx()
{
    family='MSP430FR2xx_4xx'

    # output (generated) source file
    output_file="${output_dir}/${family}/clock_pin.c"
    mkdir -p "${output_dir}/${family}"

    rm -f "${output_file}"
    bash get_specs.sh -f 'XIN' -F "${family}" -s clock -d "${output_dir}"
    bash get_specs.sh -f 'XOUT' -F "${family}" -s clock -d "${output_dir}"

    #bash get_specs.sh -f 'XIN' -T 'msp430fr2512' -s clock -d "${output_dir}"
    #bash get_specs.sh -f 'XOUT' -T 'msp430fr2512' -s clock -d "${output_dir}"
    
    for source_in in "${output_dir}"/*_clock.c; do
        source_out=${source_in//_clock.c/_clock_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head > "${output_file}"
    sha256sum ${output_dir}/*_clock_comb.c > "${output_dir}/clocks.hash"
    ifdef_comb "${output_dir}/clocks.hash" >> "${output_file}"
    out_tail_MSP430FR2xx_4xx >> "${output_file}"
}

gen_clock_MSP430FR5xx_6xx()
{
    family='MSP430FR5xx_6xx'

    # output (generated) source file
    output_file="${output_dir}/${family}/clock_pin.c"
    mkdir -p "${output_dir}/${family}"

    rm -f "${output_file}"
    bash get_specs.sh -f 'LFXIN' -tf 'crystal[ ]*mode' -F "${family}" -s clock -d "${output_dir}"
    bash get_specs.sh -f 'LFXOUT' -tf 'crystal[ ]*mode' -F "${family}" -s clock -d "${output_dir}"

    #bash get_specs.sh -f 'LFXIN' -tf 'crystal[ ]*mode' -T 'msp430fr5887' -s clock -d "${output_dir}"

    for source_in in "${output_dir}"/*_clock.c; do
        source_out=${source_in//_clock.c/_clock_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head > "${output_file}"
    sha256sum ${output_dir}/*_clock_comb.c > "${output_dir}/clocks.hash"
    ifdef_comb "${output_dir}/clocks.hash" >> "${output_file}"
    out_xt1_xt2_MSP430FR5xx_6xx >> "${output_file}"

    rm -f ${output_dir}/*.c; 
    bash get_specs.sh -f 'HFXIN' -tf 'crystal[ ]*mode' -F "${family}" -s clock -d "${output_dir}"
    bash get_specs.sh -f 'HFXOUT' -tf 'crystal[ ]*mode' -F "${family}" -s clock -d "${output_dir}"

    for source_in in "${output_dir}"/*_clock.c; do
        source_out=${source_in//_clock.c/_clock_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    sha256sum ${output_dir}/*_clock_comb.c > "${output_dir}/clocks.hash"
    ifdef_comb "${output_dir}/clocks.hash" >> "${output_file}"
    out_tail_MSP430FR5xx_6xx >> "${output_file}"
}

gen_clock_MSP430F5xx_6xx()
{
    family='MSP430F5xx_6xx'

    # output (generated) source file
    output_file="${output_dir}/${family}/clock_pin.c"
    mkdir -p "${output_dir}/${family}"

    rm -f "${output_file}"
    bash get_specs.sh -f 'XIN' -tf 'crystal[ ]*mode' -F "${family}" -s clock -d "${output_dir}"
    bash get_specs.sh -f 'XOUT' -tf 'crystal[ ]*mode' -F "${family}" -s clock -d "${output_dir}"

    #bash get_specs.sh -f 'XIN' -tf 'crystal[ ]*mode' -T 'msp430f5335' -s clock -d "${output_dir}"
    #bash get_specs.sh -f 'XOUT' -tf 'crystal[ ]*mode' -T 'msp430f5333' -s clock -d "${output_dir}"
    #bash get_specs.sh -f 'XT2IN' -tf 'crystal[ ]*mode' -T 'msp430f5510' -s clock -d "${output_dir}"
    #bash get_specs.sh -f 'XT2OUT' -tf 'crystal[ ]*mode' -T 'msp430f5510' -s clock -d "${output_dir}"

    for source_in in "${output_dir}"/*_clock.c; do
        source_out=${source_in//_clock.c/_clock_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    out_head > "${output_file}"
    sha256sum ${output_dir}/*_clock_comb.c > "${output_dir}/clocks.hash"
    ifdef_comb "${output_dir}/clocks.hash" >> "${output_file}"
    out_xt1_xt2_MSP430F5xx_6xx >> "${output_file}"

    rm -f ${output_dir}/*.c;
    bash get_specs.sh -f 'XT2IN' -tf 'crystal[ ]*mode' -F "${family}" -s clock -d "${output_dir}"
    bash get_specs.sh -f 'XT2OUT' -tf 'crystal[ ]*mode' -F "${family}" -s clock -d "${output_dir}"

    for source_in in "${output_dir}"/*_clock.c; do
        source_out=${source_in//_clock.c/_clock_comb.c}
        bitwise_comb "${source_in}" "${source_out}"
    done

    sha256sum ${output_dir}/*_clock_comb.c > "${output_dir}/clock.hash"
    ifdef_comb "${output_dir}/clock.hash" >> "${output_file}"
    out_tail_MSP430F5xx_6xx >> "${output_file}"
}

cleanup()
{
    rm -f "${output_dir}"/*_clock.c
    rm -f "${output_dir}"/*_clock_comb.c
    rm -f "${output_dir}/clock.hash"
    sync
}

gen_code()
{
    rm -f "${output_dir}"/*_clock.log
    cleanup

    gen_clock_MSP430FR2xx_4xx
    cleanup

    gen_clock_MSP430FR5xx_6xx
    cleanup

    gen_clock_MSP430F5xx_6xx
    cleanup
}

get_table_elem()
{
	uc="$1"
	elem="$2"
	ret="UNK"

	case "${elem}" in
		1)
			ret=$(grep -E '([ ]LFXIN)|([ ]XIN)[ ]pin_type' | awk '{print $NF}')
			;;
		2)
			ret=$(grep -E '([ ]LFXOUT)|([ ]XOUT)[ ]pin_type' | awk '{print $NF}')
			;;
		3)
			ret=$(grep -E '([ ]HFXIN)|([ ]XT2IN)[ ]pin_type' | awk '{print $NF}')
			;;
		4)
			ret=$(grep -E '([ ]HFXOUT)|([ ]XT2OUT)[ ]pin_type' | awk '{print $NF}')
			;;
	esac

	echo "${ret}"
}

gen_table()
{
    ucs="$(list_ucs)"
	table="${output_dir}/clock.md"

    out_head_clock_md > "${output_dir}/clock.md"

	for uc in ${ucs}; do
		echo -n "${uc}" >> "${output_dir}/clock.md"
		if [ -e "${output_dir}/${uc}_clock.log" ]; then
			for i in 1 2 3 4; do
				elem=$(get_table_elem "${uc}" "$i" < "${output_dir}/${uc}_clock.log")
				echo -n " | ${elem}" >> "${output_dir}/clock.md"
			done
			echo '' >> "${output_dir}/clock.md"
		else
			echo ' | NS | NS | NS | NS' >> "${output_dir}/clock.md"
		fi
	done
}

#gen_code
gen_table

# without USE_PARALLEL
# 270.96user 37.23system 4:21.71elapsed 117%CPU (0avgtext+0avgdata 15352maxresident)k
# 0inputs+390600outputs (281major+59455616minor)pagefaults 0swaps

# with USE_PARALLEL="true"
# 336.63user 46.62system 0:35.46elapsed 1080%CPU (0avgtext+0avgdata 17792maxresident)k
# 0inputs+936288outputs (7284major+51558214minor)pagefaults 0swaps

