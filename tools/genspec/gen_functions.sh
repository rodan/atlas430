#!/bin/bash

#  msp430 device-specific datasheet parser 
#  collection of functions that extracts pin information from pdf datasheets
#  and generates C code used to initialize the control bits for selected pins
#  Author:          Petre Rodan <2b4eda@subdimension.ro>
#  Available from:  https://github.com/rodan/reference_libs_msp430

export DATASHEET_PATH='/local/datasheets/msp430/'
export GCC_EXTRA_INC_DIR='/opt/msp430-gcc-support-files/include'

export GOOD=$'\e[32;01m'
export WARN=$'\e[33;01m'
export BAD=$'\e[31;01m'
export HILITE=$'\e[36;01m'
export BRACKET=$'\e[34;01m'
export NORMAL=$'\e[0m'

err()
{
    if [ "${DO_LOG}" == "true" ]; then
        >&2 echo " !ERR $*"
    else
        >&2 echo "${BAD} !ERR ${NORMAL} $*";
    fi
}

warn()
{
    if [ "${DO_LOG}" == "true" ]; then
        >&2 echo " WARN $*";
    else
        >&2 echo "${WARN} WARN ${NORMAL} $*";
    fi
}

inf()
{
    if [ "${DO_LOG}" == "true" ]; then
        >&2 echo "      $*"
    else
        >&2 echo "${HILITE} .... ${NORMAL} $*";
    fi
}

proc_err()
{
    >&2 echo " [${BAD}err${NORMAL}] $*"
}

# get list of ucs
list_ucs()
{
    grep 'ifeq.*430' ../../Makefile.identify-target | sed 's|.*,\(.*\))|\1|'  | tr '[:upper:]' '[:lower:]'
}

# convert 'msp430f5510' to '__MSP430F5510__'
convert_name_to_target()
{
    echo "__$(echo "${1}" | tr '[:lower:]' '[:upper:]')__"
}

# convert '__MSP430F5510__' to 'msp430f5510'
convert_target_to_name()
{
    echo "${1}" | sed 's|_||g' | tr '[:upper:]' '[:lower:]'
}

# function thet uses two algorithms to guess the family name based on a target string
# the best guess is sent to stdout
guess_family_from_target()
{
    target=$1
    header=$(grep -A1 "${target}" "${GCC_EXTRA_INC_DIR}/msp430.h" | grep '#include' | sed 's|.*include[ ]"\(.*\)".*|\1|')
    unset FAMILY_N
    unset FAMILY_S
    unset FAMILY_H
    unset FAMILY
    
    [ ! -e "${GCC_EXTRA_INC_DIR}/${header}" ] && {
        err "header ${header} is missing"
        return 1
    }
    
    # try to guess the family based on the target name
    echo "${target}" | grep -q 'CC430F5' && local FAMILY_N=MSP430F5xx_6xx
    echo "${target}" | grep -q 'MSP430F5' && local FAMILY_N=MSP430F5xx_6xx
    echo "${target}" | grep -q 'MSP430F6' && local FAMILY_N=MSP430F5xx_6xx
    echo "${target}" | grep -q 'MSP430FR2' && local FAMILY_N=MSP430FR2xx_4xx
    echo "${target}" | grep -q 'MSP430FR4' && local FAMILY_N=MSP430FR2xx_4xx
    echo "${target}" | grep -q 'MSP430FR5' && local FAMILY_N=MSP430FR5xx_6xx
    echo "${target}" | grep -q 'MSP430FR57' && local FAMILY_N=MSP430FR57xx
    echo "${target}" | grep -q 'MSP430FR6' && local FAMILY_N=MSP430FR5xx_6xx
    
    # try to get family name from the header
    FAMILY_S=$(grep '^#define.*FAMILY__' "${GCC_EXTRA_INC_DIR}/${header}" | strings)

    if [ -n "${FAMILY_S}" ]; then
        echo "${FAMILY_S}" | grep -q '__MSP430F5XX_6XX_FAMILY__' && local FAMILY_H=MSP430F5xx_6xx
        echo "${FAMILY_S}" | grep -q '__MSP430FR2XX_4XX_FAMILY__' && local FAMILY_H=MSP430FR2xx_4xx
        echo "${FAMILY_S}" | grep -q '__MSP430FR57XX_FAMILY__' && local FAMILY_H=MSP430FR57xx
        echo "${FAMILY_S}" | grep -q '__MSP430FR5XX_6XX_FAMILY__' && local FAMILY_H=MSP430FR5xx_6xx
    fi

    if [ -z "${FAMILY_N}" ] && [ -z "${FAMILY_H}" ]; then 
        return 0
    fi

    if [ "${FAMILY_N}" != "${FAMILY_H}" ]; then
        if [ -z "${FAMILY_N}" ]; then
            local FAMILY="${FAMILY_H}"
        else
            err "err family mismatch FAMILY_N = ${FAMILY_N}, FAMILY_H = ${FAMILY_H}"
        fi
    else
        FAMILY="${FAMILY_H}"
    fi

    echo "${FAMILY}"
}


# function that returns 0 if pin name is inside an interval
pin_in_interval()
{
    pin="$1"
    interval="$2"

    #inf "pin_in_interval '${pin}' '${interval}'"

    # check if port matches
    port_of_pin="${pin//\.*/}"
    port_of_interval="${interval//\.*/}"
    [ "${port_of_pin}" != "${port_of_interval}" ] && return 1

    input_pin="${pin//*\./}"
    pin_low=$(echo "$interval" | cut -d' ' -f1 | sed 's|.*\.||')
    pin_high=$(echo "$interval" | cut -d' ' -f3 | sed 's|.*\.||')

    [ "${input_pin}" -ge "${pin_low}" ] && [ "${input_pin}" -le "${pin_high}" ] && return 0
    return 1
}

# function that prints out the line which 
pin_matches_header()
{
    pin="${1}"
    port="${1//\.*/}"

    cat | while read -r line; do
        #err "${line}"
        echo "${line}" | grep -q "${pin}" && {
            #err "found by direct comparison"
            echo "${line}"
            continue
        }
        echo "${line}" | grep -q "Port ${port} Pin Functions" && {
            #err "found by direct comparison"
            echo "${line}"
            continue
        }
        if echo "${line}" | grep -q '(P[0-9J]*\.[0-7] to P[0-9J]*\.[0-7])'; then
            # shellcheck disable=SC2001
            comparison_str=$(echo "${line}" | sed 's|.*(\(P[0-9J]*\.[0-7]\) to \(P[0-9J]*\.[0-7]\)).*|\1 to \2|')
            #inf "found this comparison: $comparison_str"
            pin_in_interval "${pin}" "${comparison_str}" && {
                #err "pin was found!!!!"
                echo "${line}"
                continue
            }
        #else
        #    err "warning: pin_matches_header comparison is missing"
        fi
    done
}

# same as above, but for titles that have typos in them
pin_matches_broken_header()
{
    pin="${1}"
    port="${1//\.*/}"

    pin_no_prefix=${pin//P/}

    cat | while read -r line; do
        # remove once Table 6-47 title is fixed in https://www.ti.com/lit/gpn/msp430f5340
        echo "${line}" | grep -q "${pin_no_prefix}" && {
            echo "${line}"
            continue
        }
    done
}


# function thet uses two algorithms to guess the family name based on chip name
# the best guess is sent to stdout
guess_family_from_name()
{
    target=$(convert_name_to_target "${1}")
    guess_family_from_target "${target}"
}

# function that generates bitwise operations as C code 
out_code()
{
    register="${1}"
    bit="$2"
    value="$3"

    [ "${register}" == "ignore" ] && return 0
    #${verbose} && 
    #inf "out_code '${register}' '${bit}' '${value}'"

    case "${value}" in
        x)
            return 0
            ;;
        X)
            return 0
            ;;
        ignore)
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

# function that combines multiple bitwise operations on the same register with a single operation
# in C files generated by an earlier stage of this tool
# $1 - input source file
# $2 - output source file
bitwise_comb()
{
    input="${1}"
    output="${2}"

    true > "${output}" || {
        err "error: cannot write to output file, exiting"
        return 1
    }
    # shellcheck disable=SC2002
    sort -u "${input}" | while read -r line; do
        if echo "${line}" | grep -q 'P[0-9JSELDIR]* &= .*'; then
            register=$(echo "${line}" | grep -o 'P[0-9JSELDIR]*')
            #search for the same register with the same operator
            register_comb=$(sort -u "${input}" | grep "${register} &=")
            register_comb_lncnt=$(echo "${register_comb}" | wc -l)
            if [ "${register_comb_lncnt}" == "2" ]; then
                #extract values
                values=$(echo "${register_comb}" | awk '{ print $3 }' | sed 's|;||g' | sort -n | xargs)
                negations_cnt=$(echo "${values}" | tr -d -c '~' | wc -c)
                if [ "${negations_cnt}" == "2" ]; then
                    new_value=$(echo "${values}" | sed 's|~||g;s|[ ]| \| |')
                    new_line="    ${register} &= ~(${new_value});"
                    grep -q "^${new_line}$" "${output}" || 
                        echo "${new_line}" >> "${output}"
                else
                    echo "    ${line}" >> "${output}"
                fi
                #echo ${values}
            else
                echo "    ${line}" >> "${output}"
            fi
        elif echo "${line}" | grep -q 'P[0-9JSELDIR]* |= .*'; then
            register=$(echo "${line}" | grep -o 'P[0-9JSELDIR]*')
            #search for the same register with the same operator
            register_comb=$(sort -u "${input}" | grep "${register} |=")
            register_comb_lncnt=$(echo "${register_comb}" | wc -l)
            if [ "${register_comb_lncnt}" == "2" ]; then
                #extract values
                values=$(echo "${register_comb}" | awk '{ print $3 }' | sed 's|;||g' | sort -n | xargs)
                negations_cnt=$(echo "${values}" | tr -d -c '~' | wc -c)
                if [ "${negations_cnt}" == "0" ]; then
                    new_value=$(echo "${values}" | sed 's|[ ]| \| |')
                    new_line="    ${register} |= ${new_value};"
                    grep -q "^${new_line}$" "${output}" || 
                        echo "${new_line}" >> "${output}"
                else
                    echo "    ${line}" >> "${output}"
                fi
                #echo ${values}
            else
                echo "    ${line}" >> "${output}"
            fi

        else
            # line not recognized, output as is
            echo "${line}" >> "${output}"
        fi
    done
}


# function that combines separate source files (one for each microcontroller) into one 
# C source file that contains ifdefs
ifdef_comb()
{
    hash_file="${1}"
    unique_hashes=$(awk '{ print $1 }' "${hash_file}" | sort -u)

    first_ifdef_in_file=true
    for hash in ${unique_hashes}; do
        unset ifdef_line
        sources=$(grep "${hash}" "${hash_file}" | awk '{ print $2 }')
        first_ifdef_in_line=true
        for source in ${sources}; do
            uc_name=$(basename "${source}" | sed 's|\(.\)_.*|\1|')
            target=$(convert_name_to_target "${uc_name}")
            if ${first_ifdef_in_file}; then
                def_cnt=1
                echo -n "#if defined (${target})"
                first_ifdef_in_file=false
                first_ifdef_in_line=false
            else
                if ${first_ifdef_in_line}; then
                    def_cnt=1
                    echo -n "#elif defined (${target})"
                    first_ifdef_in_line=false
                else
                    [ "${def_cnt}" -gt 1 ] && {
                        # shellcheck disable=SC1003
                        echo ' \'
                        def_cnt=0
                    }
                    echo -n " || defined (${target})"
                    ((def_cnt+=1))
                fi
            fi
        done
        echo ''
        echo ''
        cat "${source}"
        echo ''
    done
}


export -f err
export -f warn
export -f inf
export -f proc_err
export -f list_ucs
export -f convert_name_to_target
export -f convert_target_to_name
export -f guess_family_from_target
export -f pin_in_interval
export -f pin_matches_header
export -f pin_matches_broken_header
export -f guess_family_from_name
export -f out_code
export -f bitwise_comb
export -f ifdef_comb

