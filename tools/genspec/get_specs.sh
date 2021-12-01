#!/bin/bash

#  msp430 device-specific datasheet parser 
#  collection of functions that extracts pin information from pdf datasheets
#  and generates C code used to initialize the control bits for selected pins
#  Author:          Petre Rodan <2b4eda@subdimension.ro>
#  Available from:  https://github.com/rodan/atlas430

. gen_functions.sh

#set -x

usage()
{
    cat << EOF
    Usage: $0 [] [-f function] [-h]
       -f  FUNCTION  filters out pin specifications for FUNCTION
       -tf FUNCTION  filter inside the pin table based on FUNCTION
       -F  FAMILY    filter out by family. must be one of MSP430F5xx_6xx MSP430FR2xx_4xx MSP430FR5xx_6xx MSP430FR57xx
       -t  TARGET    filter out by device target identifier
       -T  TARGET    filter out by device target regexp identifier
       -s  SUFFIX    output source file suffix    
       -d  DIRECTORY output directory
       -h           small usage text
EOF
    exit
}

# error levels:
#  0 - all ok
#  1 - error
#  2 - uc skipped
#  3 - datasheet missing
#  4 - bad column arrangement
#  5 - table parsing error
#  6 - table title not found
#  7 - pin mapping error
#  8 - function not found
process_uc()
{
    uc="$1"
    ret=1

    [ -n "${filter_target_regexp}" ] && {
        echo "${uc}" | grep -q "${filter_target_regexp}" || return 2
    }

    # try to guess the family
    FAMILY=$(guess_family_from_name "${uc}")

    [ -n "${filter_family}" ] && {
        echo "${FAMILY}" | grep -q "${filter_family}" || return 2
    }

    datasheet_pdf="${DATASHEET_PATH}/${uc}.pdf"
    datasheet_txt="${DATASHEET_PATH}/txt_layout/${uc}.txt"

    [ ! -e "${datasheet_txt}" ] && {
        err "err ${datasheet_txt} not found"
        return 3
    }

    [ ! -e "${datasheet_pdf}" ] && {
        err "err ${datasheet_pdf} not found"
        return 3
    }

    ###############################
    # get the start and end page of the chapter containing the pin data
    unset start_page
    unset end_page
    unset pdftotext_arg_first
    unset pdftotext_arg_last

    [ "${DO_LOG}" != "true" ] && echo -e "${GOOD}${uc}${NORMAL}  ${HILITE}${FAMILY}${NORMAL}"
    parse_chapters=('Input/Output Diagrams' 'Input/Output Schematics' 'Peripherals')

    for chapter in "${parse_chapters[@]}"; do
        toc=$(grep --after-context=2 --extended-regexp "${chapter}.*[\.]{20,100}" "${datasheet_txt}")
        [ -n "${toc}" ] && break
    done

    if [ -z "$toc" ]; then
        warn "ToC missing for ${uc}"
    else
        start_page=$(echo "${toc}" | grep "${chapter}[ \.]*" | sed "s|.*${chapter}[ \.]*\([0-9]*\).*|\1|")

        [ -n "${start_page}" ] && {
            end_page=$(echo "${toc}" | grep -A1 "${chapter}[ \.]*" | tail -n1 | grep -o '[0-9]*$')
            if [ -z "${end_page}" ]; then
                end_page=$(echo "${toc}" | grep -A2 "${chapter}[ \.]*" | tail -n1 | grep -o '[0-9]*$')
            else 
                [ "${end_page}" -lt "${start_page}" ] && {
                    end_page=$(echo "${toc}" | grep -A2 "${chapter}[ \.]*" | tail -n1 | grep -o '[0-9]*$')
                }
            fi
        }

        [ -n "${start_page}" ] && [ -n "${end_page}" ] && [ "${end_page}" -lt "${start_page}" ] && {
            warn "end page still reversed"
            unset end_page
        }

        ${verbose} && echo "${toc}"
        [ -n "${start_page}" ] && pdftotext_arg_first=" -f ${start_page}"
        [ -n "${end_page}" ] && pdftotext_arg_last=" -l ${end_page}"
        #inf "pdftotext '${pdftotext_arg_first}' '${pdftotext_arg_last}'"
    fi

    ###############################
    # convert (at least the pin-related chapter) and dig for the function
    unset pin_str
    unset pins
    unset function_found
    unset function_is_port_mapped
    unset function_type

    uc_spec_dump="${output_dir}/${uc}.txt"
    # shellcheck disable=SC2086
    pdftotext -layout -eol unix -nopgbrk ${pdftotext_arg_first} ${pdftotext_arg_last} "${datasheet_pdf}" "${uc_spec_dump}"

    pin_found=false
    for filter_function in ${filter_functions}; do
        function_type=''
        ${pin_found} && continue
        function_is_port_mapped=false
        pin_str=$(grep "^[ ]\{0,3\}P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./_]*${filter_function}[a-zA-Z0-9\./]*" "${uc_spec_dump}")

        # usually the pin name is the first word on a table row
        [ -z "${pin_str}" ] && pin_str=$(grep "^[ ]*P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./_]*${filter_function}[a-zA-Z0-9\./]*" "${uc_spec_dump}")

        # when the pin name is not the first word on a table row
        [ -z "${pin_str}" ] && pin_str=$(grep "[ ]*P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./_]*${filter_function}[a-zA-Z0-9\./]*" "${uc_spec_dump}")

        # when the pin name (first column data) is on multiple rows
        [ -z "${pin_str}" ] && pin_str=$(grep '^[ ]\{0,2\}[A-Z]' "${uc_spec_dump}" | awk '{ print $1 }' | sed ':x; /\/$/ { N; s|/\n|/|; tx }' | grep "^P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./_]*${filter_function}")
        if [ -n "${pin_str}" ]; then
            if echo "${pin_str}" | grep -q "PM_${filter_function}"; then
                function_is_port_mapped=true
                function_type="PM_"
            fi
            function_found="${filter_function}"
            # remove the head -n1 to parse all pins having this function
            pins=$(echo "${pin_str}" | grep -o 'P[0-9J]\{1,2\}\.[0-7]' | head -n1 | sort -u)
            pin_found=true
            inf "${filter_function} pin_type sh"
        else
            # seach dedicated pins, ignore port mapped ones
            dedicated_pin_str=$(grep -E -A65 '(Terminal Functions)|(Signal Descriptions)' "${datasheet_txt}" | grep "${filter_function}" | grep -v "PM_${filter_function}")
            #dedicated_function_found="${filter_function}"
            #dedicated_pin_found=true
        fi
    done

    if [ -z "${pin_str}" ]; then
        default_mapping_table=$(grep -E -A65 'Table.*Default Mapping' "${datasheet_txt}")
        if echo "${default_mapping_table}" | grep -q "${filter_function}"; then
            for (( i=0;i<2;i++ )); do
                after_check="-A${i}"
                before_check="-B${i}"
                pin_str=$(echo "${default_mapping_table}" | sed 's|([0-9]*)||g' | grep "${after_check}" "${before_check}" "${filter_function}" | grep '^[ ]*P[0-9J]\{1,2\}\.[0-7]')
                pin_str_lines=$(echo "${pin_str}" | wc -l)
                if [ "${pin_str_lines}" == 1 ] && [ -n "${pin_str}" ]; then 
                    pins=$(echo "${pin_str}" | grep -o 'P[0-9J]\{1,2\}\.[0-7]')
                    pin_found=true
                    function_is_port_mapped=true
                    function_found="${filter_function}"
                    function_type="default PM "
                    inf "${filter_function} pin_type pm"
                    break
                elif [ "${pin_str_lines}" -gt 1 ]; then
                    err "multiple pins found with the same distance from filter_function"
                    unset pin_str
                    ret=5
                    break
                fi
            done
        fi
    fi

    if [ -z "${pin_str}" ]; then
        if [ -z "${dedicated_pin_str}" ]; then
            warn "no dedicated, shared or port-mapped pins found for ${filter_function}"
            inf "${filter_function} pin_type -"
            echo "    #error function ${filter_function} not found for this uC" >> "${output_dir}/${uc}_${output_suffix}.c"
            ret=8
        else
            inf "${filter_function} pin_type de"
            # shellcheck disable=SC2001
            echo "${dedicated_pin_str}" | sed 's/ \+ /\t/g'
            echo "    // dedicated pin found, no setup needed, but need to dodge the catch-all #else below" >> "${output_dir}/${uc}_${output_suffix}.c"
            ret=0
        fi
    else
        ${verbose} && echo "${pin_str}"
        inf "found '${HILITE}${function_type}${function_found}${NORMAL}' as pin '${HILITE}${pins}${NORMAL}'"
    fi

    for pin in ${pins}; do
        # fish for control bits
        port=$(echo "${pin}" | grep -o 'P[0-9J]*');
        bit=$(echo "${pin}" | grep -o '[0-7]$');

        # find the table header for my particular pin
        # Port_s_ because ti_ticket_01
        # head -n1 limits the search to the first match
        table_title=$(grep "Table.*Port[s]\{0,1\} ${port}.*Pin Functions" "${uc_spec_dump}" | pin_matches_header "${pin}" | head -n1);
        [ -z "${table_title}" ] && table_title=$(grep "Table.*Port[s]\{0,1\} ${port}.*Pin Functions" "${uc_spec_dump}" | pin_matches_broken_header "${pin}" | head -n1);
        inf "table title: '${table_title}'"
        [ -z "${table_title}" ] && {
            err "table_title is NULL"
            ret=6
            continue
        }

        table_header_row=$(grep -A4 "${table_title}" "${uc_spec_dump}" | grep -E "(${port}DIR.x)|(${port}SEL)" | sed 's|([0-9]*)||g' | head -n1 | xargs)
        function_row_ignore='#############'

        # modify the header row based on local rules
        if [ "${output_suffix}" == "clock" ] && [ "${FAMILY}" == "MSP430FR5xx_6xx" ]; then
            if echo "${table_header_row}" | grep -q 'BYPA'; then
                table_header_row=$(echo "${table_header_row}" | sed 's|[LH]FXT[BYPASS]*|bypass|' | xargs)
            else
                if grep -A4 "${table_title}" "${uc_spec_dump}" | grep -q '[LH]FX[TBYPASS]'; then
                    table_header_row="${table_header_row} bypass"
                fi
            fi
            if echo "${table_header_row}" | grep -Eq '(64)|(RGC)'; then
                table_header_row=$(echo "${table_header_row}" | sed 's|RGC||;s|64||' | xargs)
            fi
        elif [ "${output_suffix}" == "clock" ] && [ "${FAMILY}" == "MSP430F5xx_6xx" ]; then
            if echo "${table_header_row}" | grep -q 'BYPA'; then
                table_header_row=$(echo "${table_header_row}" | sed 's|XT[12BYPASS]*|bypass|' | xargs)
            else
                if grep -A4 "${table_title}" "${uc_spec_dump}" | grep -q 'X[T12BYPASS]'; then
                    table_header_row="${table_header_row} bypass"
                fi
            fi
        fi

        # generic rules
        if [ "${FAMILY}" == "MSP430FR2xx_4xx" ]; then
            function_row_ignore='TB3'
            if echo "${table_header_row}" | grep -q 'ANALOG' || echo "${table_header_row}" | grep -q 'FUNCTION'; then
                table_header_row=$(echo "${table_header_row}" | sed 's|ANALOG.*FUNCTION|analog|' | xargs)
            else
                if grep -A4 "${table_title}" "${uc_spec_dump}" | grep -q 'ANALOG'; then
                    # 'analog function' should end up into the second-to-last position before any JTAG
                    if echo "${table_header_row}" | grep -q 'JTAG'; then
                        table_header_row=$(echo "${table_header_row}" | sed 's|JTAG|analog JTAG|' | xargs)
                    else
                        table_header_row="${table_header_row} analog"
                    fi
                fi
            fi
        fi

        if [ "${FAMILY}" == "MSP430FR5xx_6xx" ]; then
            if echo "${table_header_row}" | grep -Eq '(64)|(RGC)'; then
                table_header_row=$(echo "${table_header_row}" | sed 's|RGC||;s|64||' | xargs)
            fi
        fi

        if [ "${FAMILY}" == "MSP430F5xx_6xx" ]; then
            if echo "${table_header_row}" | grep -q 'LCDS'; then
                table_header_row=$(echo "${table_header_row}" | sed 's|LCDS[0-9\.]*|lcds|' | sed 's|to [0-9]*||' | xargs)
            else
                if grep -A4 "${table_title}" "${uc_spec_dump}" | grep -q 'LCDS'; then
                    table_header_row="${table_header_row} lcds"
                fi
            fi

            if echo "${table_header_row}" | grep -q 'COM'; then
                : # did not happen yet
            else
                if grep -A4 "${table_title}" "${uc_spec_dump}" | grep -q 'COM'; then
                    table_header_row="${table_header_row} com"
                fi
            fi
        fi

        columns_cnt=$(echo "${table_header_row}" | wc -w)
        unset bit_override

        # keep only the relevant columns (DIR and SEL)
        for ((i=1; i<=columns_cnt; i++)); do
            column_def=$(echo "${table_header_row}" | awk "{ print \$${i} }")
            if echo "${column_def}" | grep -q "${port}DIR.x"; then
                column[$i]=${port}DIR
            elif echo "${column_def}" | grep -q "${port}SELx"; then
                column[$i]=${port}SEL
            elif echo "${column_def}" | grep -q "${port}SEL.x"; then
                column[$i]=${port}SEL
            elif echo "${column_def}" | grep -q "${port}SEL0.x"; then
                column[$i]=${port}SEL0
            elif echo "${column_def}" | grep -q "${port}SEL1.x"; then
                column[$i]=${port}SEL1
            elif echo "${column_def}" | grep -q "${port}SEL.[0-7]"; then
                bit_override[$i]=$(echo "${column_def}" | cut -d'.' -f2)
                column[$i]=${port}SEL
            elif echo "${column_def}" | grep -q "${port}SEL0.[0-7]"; then
                bit_override[$i]=$(echo "${column_def}" | cut -d'.' -f2)
                column[$i]=${port}SEL0
            elif echo "${column_def}" | grep -q "${port}SEL1.[0-7]"; then
                bit_override[$i]=$(echo "${column_def}" | cut -d'.' -f2)
                column[$i]=${port}SEL1
            elif echo "${column_def}" | grep -q "ANALOG"; then
                column[$i]=ANALOG
            else
                column[$i]=ignore
            fi
            #inf "column[$i] = ${column[$i]}"
        done

        multi_function=false
        echo "${function_found}" | grep -q 'UC[AB][0-3]' && multi_function=true

        # filter the row containing the function, make sure to ignore all footnotes
        if [ -n "${filter_table_function}" ]; then
            if [ "${multi_function}" == "true" ]; then
                function_row=$(grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "^[A-Z0-9/ \._]\{25,\}${function_found}" | grep "[ ]${filter_table_function}[ ]" | grep -Ev "${function_row_ignore}" | xargs)
            else
                function_row=$(grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "[ ]${function_found}[ ]" | grep "[ ]${filter_table_function}[ ]" | grep -Ev "${function_row_ignore}" | xargs)
            fi
            function_row_data=$(echo "${function_row}" | sed "s|.*${function_found}||g;s|.*${filter_table_function}||g" | xargs)
        else
            if [ "${multi_function}" == "true" ]; then
                # xargs -0 needed due to typo containing an apostrophe in msp430fr2433.pdf ti_ticket_02
                function_row=$(grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "^[A-Z0-9/ \._!]\{25,\}${function_found}" | grep -Ev "${function_row_ignore}" | head -n1 |xargs -0)
                function_row_data=$(echo "${function_row}" | sed "s|.*${function_found}[A-Z0-9/\.'_]*||g" | xargs)
            else
                function_row=$(grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "[ ]${function_found}[ ]" | grep -Ev "${function_row_ignore}" | xargs)
                function_row_data=$(echo "${function_row}" | sed "s|.*${function_found}||g" | xargs)
            fi
        fi
        function_row_data_cnt=$(echo "${function_row_data}" | wc -w)

        if [ "${function_row_data_cnt}" == 0 ] && [ "${function_is_port_mapped}" == "true" ]; then
            # might need to add an if [ -n "${filter_table_function}" ]
            for (( i=0;i<3;i++ )); do
                after_check="-A${i}"
                before_check="-B${i}"
                function_row_data=$(grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "${after_check}" "${before_check}" "[ ]\{0,10\}${pin}" | grep "Mapped secondary digital" | sed 's|0; 1|ignore|g;s|.*Table [0-9]*-[0-9]*||g;s|≤ [0-9]*|ignore|g;s|= [0-9]*|ignore|g;s|.*Mapped secondary digita[l function]\{1,10\}||;s|[()]||')
                function_rows=$(echo "${function_row_data}" | wc -l)
                if [ "${function_rows}" -gt 1 ]; then
                    function_row_data=$(echo "${function_row_data}" | head -n1 | xargs)
                    function_row_data2=$(echo "${function_row_data}" | tail -n1 | xargs)
                    if [ "${function_row_data}" != "${function_row_data2}" ]; then
                        err "mapping of secondary digital function ${pin}/${function_found} differ between IC packages"
                        ret=7
                    fi
                else
                    function_row_data=$(echo "${function_row_data}" | xargs)
                fi
                function_row_data_cnt=$(echo "${function_row_data}" | wc -w)
                #grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "${after_check}" "${before_check}" "${pin}"
                [ "${function_row_data_cnt}" -gt 0 ] && break;
            done
            if [ "${function_row_data_cnt}" == 0 ]; then
                inf "concatenating mapped data with next row"
                 for (( i=0;i<6;i++ )); do
                    after_check="-A${i}"
                    before_check="-B${i}"
                    function_row_data_tmp=$(grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "${after_check}" "${before_check}" "[ ]\{0,10\}${pin}")
                    #function_row_data=$(grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "${after_check}" "${before_check}" "[ ]\{0,10\}${pin}" | grep -A1 "Mapped secondary digital" | sed 's|0; 1|ignore|g;s|.*Table [0-9]*-[0-9]*|             |g;s|≤ [0-9]*|ignore|g;s|= [0-9]*|ignore|g;s|.*[Mapped secondary d]igita[l function]\{1,10\}|                 |;s|[()]||' | cut -c41- | xargs)
                    if echo "${function_row_data_tmp}" | grep -q 'Mapped secondary digital' ; then
                        function_row_data=$(echo "${function_row_data_tmp}" | grep -A1 "Mapped secondary digital" | sed 's|0; 1|ignore|g;s|.*Table [0-9]*-[0-9]*|             |g;s|≤ [0-9]*|ignore|g;s|= [0-9]*|ignore|g;s|.*[Mapped secondary d]igita[l function]\{1,10\}|                 |;s|[()]||' | cut -c41- | xargs)
                    elif echo "${function_row_data_tmp}" | cut -c25- | grep -q "${function_found}"; then
                        function_row_data=$(echo "${function_row_data_tmp}" | grep -A1 "${function_found}" | cut -c65- | xargs)
                    fi
                    function_row_data_cnt=$(echo "${function_row_data}" | wc -w)
                    #grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "${after_check}" "${before_check}" "[ ]\{0,10\}${pin}"
                    [ "${function_row_data_cnt}" -gt 0 ] && break;
                done               
            fi
        elif [ "${function_row_data_cnt}" == 0 ]; then
            # sometimes the data for the function is shifted one row lower due to limited tabular cell size
            # data is likely missing from this row, concatenate with the next one
            inf "concatenating data with next row"
            for (( i=0;i<6;i++ )); do
                after_check="-A${i}"
                before_check="-B${i}"
                function_row_data_tmp=$(grep -A60 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "${after_check}" "${before_check}" "[ ]\{0,10\}${pin}")
                if echo "${function_row_data_tmp}" | cut -c25- | grep -q "${function_found}"; then
                    if [ -n "${filter_table_function}" ]; then
                        function_row_data=$(echo "${function_row_data_tmp}" | cut -c25- | grep -A1 "${function_found}" | grep -A1 "${filter_table_function}" | cut -c40- | xargs)
                    else
                        function_row_data=$(echo "${function_row_data_tmp}" | cut -c25- | grep -A1 "${function_found}" | cut -c40- | xargs )
                    fi
                fi
                function_row_data_cnt=$(echo "${function_row_data}" | wc -w)
                [ "${function_row_data_cnt}" -gt 0 ] && break;
            done
        fi

        column_diff=$(( columns_cnt - function_row_data_cnt ))
        if [ "${column_diff}" == "1" ]; then
            if [ "${column[$columns_cnt]}" == "ignore" ]; then
                function_row_data="${function_row_data} padded"
                function_row_data_cnt=$(echo "${function_row_data}" | wc -w)
            fi
        fi

        function_row_data_display=$(echo "${function_row_data}" | sed "s|padded|${WARN}padded${NORMAL}|g;s|ignore|${WARN}ignore${NORMAL}|g")
        cat << EOF | column -t -s' '
${table_header_row}
${function_row_data_display}
EOF

        # shellcheck disable=SC2129
        if [ "${columns_cnt}" == "${function_row_data_cnt}" ]; then
            for ((i=1; i<=columns_cnt; i++)); do
                value[$i]=$(echo "${function_row_data}" | awk "{ print \$${i} }");
                unset tbit
                if [[ ${bit_override[$i]} == ?(-)+([0-9]) ]]; then
                    tbit="${bit_override[$i]}"
                else
                    tbit="${bit}"
                fi
                out_code "${column[$i]}" "${tbit}" "${value[$i]}" >> "${output_dir}/${uc}_${output_suffix}.c"
            done
        else
            err "${BAD}error: unknown column arrangement${NORMAL} col:${columns_cnt} data:${function_row_data_cnt}"
            return 4
        fi
        cat "${output_dir}/${uc}_${output_suffix}.c" && ret=0
    done

    return "${ret}"
}

redirect_process()
{
    uc="$1"
    log_file="${output_dir}/${uc}_${output_suffix}.log"

    process_uc "${uc}" >> "${log_file}_" 2>&1
    case $? in
        1)
            proc_err "${uc}(${filter_functions}): unknown error encountered"
            ;;
        2)
            rm -f "${log_file}_"
            ;;
        3)
            proc_err "${uc}(${filter_functions}): datasheet is missing"
            ;;
        4)
            proc_err "${uc}(${filter_functions}): bad column arrangement"
            cat "${log_file}_"
            ;;
        5)
            proc_err "${uc}(${filter_functions}): table parsing error"
            cat "${log_file}_"
            ;;
        6)
            proc_err "${uc}(${filter_functions}): table title not found"
            cat "${log_file}_"
            ;;
        7)
            proc_err "${uc}(${filter_functions}): pin mapping error"
            cat "${log_file}_"
            ;;
        8)
            proc_err "${uc}(${filter_functions}): function not found"
            ;;
    esac

    [ -e "${log_file}_" ] && {
        cat "${log_file}_" >> "${log_file}"
        rm -f "${log_file}_"
    }
}

export -f process_uc
export -f redirect_process

verbose=false
while (( "$#" )); do
	if [ "$1" = "-f" ]; then
        filter_functions="${2}"
		shift; shift;
	elif [ "$1" = "-tf" ]; then
        filter_table_function="${2}"
		shift; shift;
	elif [ "$1" = "-F" ]; then
        filter_family="${2}"
		shift; shift;
	elif [ "$1" = "-t" ]; then
        filter_target="${2}"
		shift; shift;
	elif [ "$1" = "-s" ]; then
        output_suffix="${2}"
		shift; shift;
	elif [ "$1" = "-d" ]; then
        output_dir="${2}"
		shift; shift;
	elif [ "$1" = "-T" ]; then
        filter_target_regexp="${2}"
		shift; shift;
	elif [ "$1" = "-v" ]; then
        verbose=true
        shift;
    else
		shift;
		usage
    fi
done

[ -z "${filter_functions}" ] && {
    err "no '-f FUNCTION' was specified, exiting"
    usage
}

[ -z "${output_dir}" ] && {
    err "error: no '-d DIRECTORY' was specified, exiting"
    usage
}

mkdir -p "${output_dir}" || {
    err "error: unable to create output directory '${output_dir}'"
    exit 1
}

export filter_functions
export filter_table_function
export filter_family
export filter_target
export output_suffix
export output_dir
export filter_target_regexp
export verbose

if [ -n "${filter_target}" ]; then
    ucs="${filter_target}"
else
    ucs="$(list_ucs)"
fi

USE_PARALLEL='true'
DO_LOG='false'

if [ "${USE_PARALLEL}" == "true" ]; then
    export DO_LOG='true'
    echo "${ucs}" | parallel -j+0 redirect_process
else
    for i in ${ucs}; do
        if [ "${DO_LOG}" == "true" ]; then
            redirect_process "$i"
        else
            process_uc "$i"
        fi
    done
fi

