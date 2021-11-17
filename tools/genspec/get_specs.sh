#!/bin/bash

. gen_functions.sh

usage()
{
    cat << EOF
    Usage: $0 [] [-f function] [-h]
       -f FUNCTION  filters out pin specifications for FUNCTION
       -F FAMILY    filter out by family. must be one of MSP430F5xx_6xx MSP430FR2xx_4xx MSP430FR5xx_6xx MSP430FR57xx
       -t TARGET    filter out by device target identifier
       -T TARGET    filter out by device target regexp identifier
       -s SUFFIX    output source file suffix    
       -d DIRECTORY output directory
       -h           small usage text
EOF
    exit
}

out_code()
{
    register="${1}"
    bit="$2"
    value="$3"

    #${verbose} && 
    err "out_code '${register}' '${bit}' '${value}'"

    case "${value}" in
        x)
            return 0
            ;;
        X)
            return 0
            ;;
        0)
            echo "    ${register} &= ~BIT${bit};"
            ;;
        1)
            echo "    ${register} |= BIT${bit};"
            ;;
        00)
            echo "${register}" | grep -q 'SEL' || {
                err "error: unknown value '${value}' for register '${register}' '${bit}'"
                return 1
            }
            echo "    ${register}0 &= ~BIT${bit};"
            echo "    ${register}1 &= ~BIT${bit};"
            ;;
        01)
            echo "${register}" | grep -q 'SEL' || {
                err "error: unknown value '${value}' for register '${register}' '${bit}'"
                return 1
            }
            echo "    ${register}0 |= BIT${bit};"
            echo "    ${register}1 &= ~BIT${bit};"
            ;;
        10)
            echo "${register}" | grep -q 'SEL' || {
                err "error: unknown value '${value}' for register '${register}' '${bit}'"
                return 1
            }
            echo "    ${register}0 &= ~BIT${bit};"
            echo "    ${register}1 |= BIT${bit};"
            ;;
        11)
            echo "${register}" | grep -q 'SEL' || {
                err "error: unknown value '${value}' for register '${register}' '${bit}'"
                return 1
            }
            echo "    ${register}0 |= BIT${bit};"
            echo "    ${register}1 |= BIT${bit};"
            ;;
        *)
            err "error: unknown value '${value}' for register '${register}' '${bit}'"
            return 1
            ;;
    esac
}

verbose=false

while (( "$#" )); do
	if [ "$1" = "-f" ]; then
        filter_functions="${2}"
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

    chapter='Input/Output Diagrams'
    echo -e "${GOOD}${uc}${NORMAL}  ${HILITE}${FAMILY}${NORMAL}"
    toc=$(grep --after-context=2 --extended-regexp "${chapter}.*[\.]{20,100}" "${datasheet_txt}")
    if [ -z "${toc}" ]; then 
        chapter='Peripherals'
        toc=$(grep --after-context=2 --extended-regexp "${chapter}.*[\.]{20,100}" "${datasheet_txt}")
    fi

    if [ -z "$toc" ]; then
        err "${BAD}toc still empty for ${uc}${NORMAL}"
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
        #echo "${WARN}'${pdftotext_arg_first}'${NORMAL}"
    fi

    ###############################
    # convert (at least the pin-related chapter) and dig for the function
    unset pin_str
    unset pins
    unset function_found

    uc_spec_dump="${output_dir}/${uc}.txt"
    pdftotext -layout -eol unix -nopgbrk ${pdftotext_arg_first} ${pdftotext_arg_last} "${datasheet_pdf}" "${uc_spec_dump}"

    pin_found=false
    for filter_function in ${filter_functions}; do
        ${pin_found} && continue
        pin_str=$(grep "^[ ]\{0,3\}P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./]*${filter_function}[a-zA-Z0-9\./]*" "${uc_spec_dump}")
        [ -z "${pin_str}" ] && pin_str=$(grep "^[ ]*P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./]*${filter_function}[a-zA-Z0-9\./]*" "${uc_spec_dump}")
        [ -z "${pin_str}" ] && pin_str=$(grep "[ ]*P[0-9J]\{1,2\}\.[0-7]/[a-zA-Z0-9\./]*${filter_function}[a-zA-Z0-9\./]*" "${uc_spec_dump}")
        if [ -n "${pin_str}" ]; then
            pins=$(echo "${pin_str}" | grep -o 'P[0-9J]\{1,2\}\.[0-7]' | sort -u)
            function_found="${filter_function}"
            pin_found=true
        fi
    done

    if [ -z "${pin_str}" ]; then
        err "${BAD}error: functions not found in ${uc}${NORMAL}"
    else
        ${verbose} && echo "${pin_str}"
        echo " found '${function_found}' as pin '${HILITE}${pins}${NORMAL}'"
    fi

    for pin in ${pins}; do
        # fish for control bits
        port=$(echo "${pin}" | grep -o 'P[0-9J]*');
        bit=$(echo "${pin}" | grep -o '[0-7]$');
        control_line=$(grep -A4 "Table.*Port ${port}.*Pin Functions" "${uc_spec_dump}" | grep -E "(${port}DIR.x)|(${port}SEL)" | head -n1)
        columns_cnt=$(echo "${control_line}" | wc -w)

        # keep only relevant columns (DIR and SEL)
        for ((i=1; i<=columns_cnt; i++)); do
            column_def=$(echo "${control_line}" | awk "{ print \$${i} }")
            echo "${column_def}" | grep -q "${port}DIR.x" && column[$i]=${port}DIR
            echo "${column_def}" | grep -q "${port}SELx" && column[$i]=${port}SEL
            echo "${column_def}" | grep -q "${port}SEL0.x" && column[$i]=${port}SEL0
            echo "${column_def}" | grep -q "${port}SEL1.x" && column[$i]=${port}SEL1
            echo "${column_def}" | grep -q "ANALOG" && column[$i]=ANALOG
        done

        #echo "${columns_cnt}"
        #[ "${columns_cnt}" -gt 2 ] && echo "$control_line" 

        function_line=$(grep -A50 "Table.*Port ${port}.*Pin Functions" "${uc_spec_dump}" | grep "[ ]${function_found}[ ]")
        function_line_cnt=$(echo "${function_line}" | wc -w)
      
        #echo "${columns_cnt} ${function_line_cnt}"
        #echo "$control_line" 
        #echo "${function_line}"

        if [ "${columns_cnt}" == 2 ] && [ "${function_line_cnt}" == 3 ]; then
            value[1]=$(echo "${function_line}" | awk '{ print $2 }');
            value[2]=$(echo "${function_line}" | awk '{ print $3 }');
            out_code "${column[1]}" "${bit}" "${value[1]}" >> "${output_dir}/${uc}_${output_suffix}.c"
            out_code "${column[2]}" "${bit}" "${value[2]}" >> "${output_dir}/${uc}_${output_suffix}.c"
        elif [ "${columns_cnt}" == 2 ] && [ "${function_line_cnt}" == 4 ]; then
            # if in the table header the last column's name is interlaced and missing from the found line
            # quite risky
            value[1]=$(echo "${function_line}" | awk '{ print $2 }');
            value[2]=$(echo "${function_line}" | awk '{ print $3 }');
            out_code "${column[1]}" "${bit}" "${value[1]}" >> "${output_dir}/${uc}_${output_suffix}.c"
            out_code "${column[2]}" "${bit}" "${value[2]}" >> "${output_dir}/${uc}_${output_suffix}.c"
        elif [ "${columns_cnt}" == 4 ] && [ "${function_line_cnt}" == 4 ] && [ "${column[3]}" == "ANALOG" ]; then
            value[1]=$(echo "${function_line}" | awk '{ print $2 }');
            value[2]=$(echo "${function_line}" | awk '{ print $3 }');
            out_code "${column[1]}" "${bit}" "${value[1]}" >> "${output_dir}/${uc}_${output_suffix}.c"
            out_code "${column[2]}" "${bit}" "${value[2]}" >> "${output_dir}/${uc}_${output_suffix}.c"
        else
            err "${BAD}error: unknown column arrangement${NORMAL}"
        fi
    done

done
