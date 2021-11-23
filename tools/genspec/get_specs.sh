#!/bin/bash

. gen_functions.sh

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

if [ -n "${filter_target}" ]; then
    ucs="${filter_target}"
else
    ucs=$(grep ifeq ../..//Makefile.identify-target | sed 's|.*,\(.*\))|\1|'  | tr '[:upper:]' '[:lower:]')
fi

for uc in ${ucs}; do

     [ -n "${filter_target_regexp}" ] && {
        echo "${uc}" | grep -q "${filter_target_regexp}" || continue
    }

    # try to guess the family
    FAMILY=$(guess_family_from_name "${uc}")

    [ -n "${filter_family}" ] && {
        echo "${FAMILY}" | grep -q "${filter_family}" || continue
    }

    datasheet_pdf="${DATASHEET_PATH}/${uc}.pdf"
    datasheet_txt="${DATASHEET_PATH}/txt_layout/${uc}.txt"

    [ ! -e "${datasheet_txt}" ] && {
        err "err ${datasheet_txt} not found"
        continue
    }

    [ ! -e "${datasheet_pdf}" ] && {
        err "err ${datasheet_pdf} not found"
        continue
    }

    ###############################
    # get the start and end page of the chapter containing the pin data
    unset start_page
    unset end_page
    unset pdftotext_arg_first
    unset pdftotext_arg_last

    echo -e "${GOOD}${uc}${NORMAL}  ${HILITE}${FAMILY}${NORMAL}"
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
            err "${BAD}end page still reversed for ${uc}${NORMAL}"
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

    uc_spec_dump="${output_dir}/${uc}.txt"
    # shellcheck disable=SC2086
    pdftotext -layout -eol unix -nopgbrk ${pdftotext_arg_first} ${pdftotext_arg_last} "${datasheet_pdf}" "${uc_spec_dump}"

    pin_found=false
    for filter_function in ${filter_functions}; do
        ${pin_found} && continue
        pin_str=$(grep "^[ ]\{0,3\}P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./]*${filter_function}[a-zA-Z0-9\./]*" "${uc_spec_dump}")

        # usually the pin name is the first word on a table row
        [ -z "${pin_str}" ] && pin_str=$(grep "^[ ]*P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./]*${filter_function}[a-zA-Z0-9\./]*" "${uc_spec_dump}")

        # when the pin name is not the first word on a table row
        [ -z "${pin_str}" ] && pin_str=$(grep "[ ]*P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./]*${filter_function}[a-zA-Z0-9\./]*" "${uc_spec_dump}")

        # when the pin name (first column data) is on multiple lines
        [ -z "${pin_str}" ] && pin_str=$(grep '^[ ]\{0,2\}[A-Z]' "${uc_spec_dump}" | awk '{ print $1 }' | sed ':x; /\/$/ { N; s|/\n|/|; tx }' | grep "^P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./]*${filter_function}")
        if [ -n "${pin_str}" ]; then
            # remove the head -n1 to parse all pins having this function
            pins=$(echo "${pin_str}" | grep -o 'P[0-9J]\{1,2\}\.[0-7]' | head -n1 | sort -u)
            function_found="${filter_function}"
            pin_found=true
        else
            # seach dedicated pins
            dedicated_pin_str=$(grep -E -A65 '(Terminal Functions)|(Signal Descriptions)' "${datasheet_txt}" | grep "${filter_function}")
            dedicated_function_found="${filter_function}"
            dedicated_pin_found=true
        fi
    done

    if [ -z "${pin_str}" ]; then
        warn "warning: no shared pins with '${filter_functions}' function(s) found"
        if [ -z "${dedicated_pin_str}" ]; then
            err "error: no dedicated pins with those functions have been found"
        else
            echo "${dedicated_pin_str}" | sed 's/ \+ /\t/g'
            echo "    // dedicated pin found, no setup needed, but need to dodge the catch-all #else below" > "${output_dir}/${uc}_${output_suffix}.c"
        fi
    else
        ${verbose} && echo "${pin_str}"
        inf "found '${function_found}' as pin '${HILITE}${pins}${NORMAL}'"
    fi

    for pin in ${pins}; do
        # fish for control bits
        port=$(echo "${pin}" | grep -o 'P[0-9J]*');
        bit=$(echo "${pin}" | grep -o '[0-7]$');

        # find the table header for my particular pin
        table_title=$(grep "Table.*Port ${port}.*Pin Functions" "${uc_spec_dump}" | pin_matches_header "${pin}");
        [ -z "${table_title}" ] && table_title=$(grep "Table.*Port ${port}.*Pin Functions" "${uc_spec_dump}" | pin_matches_broken_header "${pin}");
        inf "table title: '${table_title}'"
        [ -z "${table_title}" ] && {
            err "table_title is NULL"
            continue
        }

        table_header_line=$(grep -A4 "${table_title}" "${uc_spec_dump}" | grep -E "(${port}DIR.x)|(${port}SEL)" | sed 's|([0-9]*)||g' | head -n1 | xargs)
        function_line_ignore='#############'

        # modify the header line based on local rules
        if [ "${output_suffix}" == "clock" ] && [ "${FAMILY}" == "MSP430FR2xx_4xx" ]; then
            if echo "${table_header_line}" | grep -q 'ANALOG' || echo "${table_header_line}" | grep -q 'FUNCTION'; then
                table_header_line=$(echo "${table_header_line}" | sed 's|ANALOG.*FUNCTION|analog|' | xargs)
            else
                if grep -A4 "${table_title}" "${uc_spec_dump}" | grep -q 'ANALOG'; then
                    table_header_line="${table_header_line} analog"
                fi
            fi
        elif [[ "${output_suffix}" == "uart"* ]] && [ "${FAMILY}" == "MSP430FR2xx_4xx" ]; then
            function_line_ignore='TB3'
            if echo "${table_header_line}" | grep -q 'ANALOG' || echo "${table_header_line}" | grep -q 'FUNCTION'; then
                table_header_line=$(echo "${table_header_line}" | sed 's|ANALOG.*FUNCTION|analog|' | xargs)
            else
                if grep -A4 "${table_title}" "${uc_spec_dump}" | grep -q 'ANALOG'; then
                    # 'analog function' should end up into the second-to-last position before any JTAG
                    if echo "${table_header_line}" | grep -q 'JTAG'; then
                        table_header_line=$(echo "${table_header_line}" | sed 's|JTAG|analog JTAG|' | xargs)
                    else
                        table_header_line="${table_header_line} analog"
                    fi
                fi
            fi
        elif [ "${output_suffix}" == "clock" ] && [ "${FAMILY}" == "MSP430FR5xx_6xx" ]; then
            if echo "${table_header_line}" | grep -q 'BYPA'; then
                table_header_line=$(echo "${table_header_line}" | sed 's|[LH]FXT[BYPASS]*|bypass|' | xargs)
            else
                if grep -A4 "${table_title}" "${uc_spec_dump}" | grep -q '[LH]FX[TBYPASS]'; then
                    table_header_line="${table_header_line} bypass"
                fi
            fi
            if echo "${table_header_line}" | grep -Eq '(64)|(RGC)'; then
                table_header_line=$(echo "${table_header_line}" | sed 's|RGC||;s|64||' | xargs)
            fi
        elif [ "${output_suffix}" == "clock" ] && [ "${FAMILY}" == "MSP430F5xx_6xx" ]; then
            if echo "${table_header_line}" | grep -q 'BYPA'; then
                table_header_line=$(echo "${table_header_line}" | sed 's|XT[12BYPASS]*|bypass|' | xargs)
            else
                if grep -A4 "${table_title}" "${uc_spec_dump}" | grep -q 'X[T12BYPASS]'; then
                    table_header_line="${table_header_line} bypass"
                fi
            fi
        fi

        # generic rules
        if [ "${FAMILY}" == "MSP430FR5xx_6xx" ]; then
            if echo "${table_header_line}" | grep -Eq '(64)|(RGC)'; then
                table_header_line=$(echo "${table_header_line}" | sed 's|RGC||;s|64||' | xargs)
            fi
        fi

        columns_cnt=$(echo "${table_header_line}" | wc -w)
        unset bit_override

        # keep only the relevant columns (DIR and SEL)
        for ((i=1; i<=columns_cnt; i++)); do
            column_def=$(echo "${table_header_line}" | awk "{ print \$${i} }")
            if echo "${column_def}" | grep -q "${port}DIR.x"; then
                column[$i]=${port}DIR
            elif echo "${column_def}" | grep -q "${port}SELx"; then
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

        # filter the line containing the function, make sure to ignore all footnotes
        if [ -n "${filter_table_function}" ]; then
            if [ "${multi_function}" == "true" ]; then
                function_line=$(grep -A50 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "^[A-Z0-9/ \.]\{25,\}${function_found}" | grep "[ ]${filter_table_function}[ ]" | grep -Ev "${function_line_ignore}" | xargs)
            else
                function_line=$(grep -A50 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "[ ]${function_found}[ ]" | grep "[ ]${filter_table_function}[ ]" | grep -Ev "${function_line_ignore}" | xargs)
            fi
            function_line_data=$(echo "${function_line}" | sed "s|.*${function_found}||g;s|.*${filter_table_function}||g" | xargs)
        else
            if [ "${multi_function}" == "true" ]; then
                # xargs -0 needed due to typo containing an apostrophe in msp430fr2433.pdf
                function_line=$(grep -A50 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "^[A-Z0-9/ \.]\{25,\}${function_found}" | grep -Ev "${function_line_ignore}" | head -n1 |xargs -0)
                function_line_data=$(echo "${function_line}" | sed "s|.*${function_found}[A-Z0-9/\.']*||g" | xargs)
                echo ${function_line}
            else
                function_line=$(grep -A50 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep "[ ]${function_found}[ ]" | grep -Ev "${function_line_ignore}" | xargs)
                function_line_data=$(echo "${function_line}" | sed "s|.*${function_found}||g" | xargs)
            fi
        fi
        function_line_data_cnt=$(echo "${function_line_data}" | wc -w)
        # sometimes the data for the function is shifted one line lower due to limited tabular cell size
        if [ "${function_line_data_cnt}" == 0 ]; then
            # data is likely missing from this line, concatenate with the next one
            inf "concatenating data with next line"
            if [ -n "${filter_table_function}" ]; then
                function_line=$(grep -A50 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep -A1 "[ ]${function_found}[ ]" | grep -A1 "${filter_table_function}" | xargs)
            else
                function_line=$(grep -A50 "${table_title}" "${uc_spec_dump}" | sed 's|([0-9]*)||g' | grep -A1 "[ ]${function_found}[ ]" | xargs )
            fi
            function_line_data=$(echo "${function_line}" | sed "s|.*${function_found}||g;s|.*${filter_table_function}||g" | xargs)
            function_line_data_cnt=$(echo "${function_line_data}" | wc -w)
        fi

        

        cat << EOF | column -t -s' '
${table_header_line}
${function_line_data}
EOF

        # shellcheck disable=SC2129
        if [ "${columns_cnt}" == "${function_line_data_cnt}" ]; then
            for ((i=1; i<=columns_cnt; i++)); do
                value[$i]=$(echo "${function_line_data}" | awk "{ print \$${i} }");
                unset tbit
                if [[ ${bit_override[$i]} == ?(-)+([0-9]) ]]; then
                    tbit="${bit_override[$i]}"
                else
                    tbit="${bit}"
                fi
                out_code "${column[$i]}" "${tbit}" "${value[$i]}" >> "${output_dir}/${uc}_${output_suffix}.c"
            done
        else
            err "${BAD}error: unknown column arrangement${NORMAL} col:${columns_cnt} data:${function_line_data_cnt}"
        fi
        cat "${output_dir}/${uc}_${output_suffix}.c"
    done

done

