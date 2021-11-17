
DATASHEET_PATH='/local/datasheets/msp430/'
GCC_EXTRA_INC_DIR='/opt/msp430-gcc-support-files/include'

err()
{
    >&2 echo $@; 
}

GOOD=$'\e[32;01m'
WARN=$'\e[33;01m'
BAD=$'\e[31;01m'
HILITE=$'\e[36;01m'
BRACKET=$'\e[34;01m'
NORMAL=$'\e[0m'

# convert 'msp430f5510' to '__MSP430F5510__'
convert_name_to_target()
{
    echo "__$(echo "${1}" | tr '[:lower:]' '[:upper:]')__"
}

# convert '__MSP430F5510__' to 'msp430f5510'
convert_target_to_name()
{
    echo "$(echo "${1}" | sed 's|_||g' | tr '[:upper:]' '[:lower:]')"
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

    if [ ! -z "${FAMILY_S}" ]; then
        echo "${FAMILY_S}" | grep -q '__MSP430F5XX_6XX_FAMILY__' && local FAMILY_H=MSP430F5xx_6xx
        echo "${FAMILY_S}" | grep -q '__MSP430FR2XX_4XX_FAMILY__' && local FAMILY_H=MSP430FR2xx_4xx
        echo "${FAMILY_S}" | grep -q '__MSP430FR57XX_FAMILY__' && local FAMILY_H=MSP430FR57xx
        echo "${FAMILY_S}" | grep -q '__MSP430FR5XX_6XX_FAMILY__' && local FAMILY_H=MSP430FR5xx_6xx
    fi

    if [ -z "${FAMILY_N}"  -a -z "${FAMILY_H}" ]; then 
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

# function thet uses two algorithms to guess the family name based on chip name
# the best guess is sent to stdout
guess_family_from_name()
{
    target=$(convert_name_to_target "${1}")
    guess_family_from_target "${target}"
}

