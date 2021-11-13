#!/bin/bash

set -e

usage()
{
    echo "available commands:"
}

GCC_EXTRA_INC_DIR='/opt/msp430-gcc-support-files/include'

err()
{
    >&2 echo $@; 
}

get_header_version()
{
    [ ! -e "${GCC_EXTRA_INC_DIR}/../Revisions_Header.txt" ] && return 0
    HEADER_VER=$(grep -A3 '^Build' "${GCC_EXTRA_INC_DIR}/../Revisions_Header.txt" | strings | head -n3)
    HEADER_BUILD=$(echo "${HEADER_VER}" | head -n1 | grep 'Build' | sed 's|Build[ ]||')
    HEADER_DATE=$(echo "${HEADER_VER}" | tail -n1  | grep '.*/.*/.*')
}

print_head()
{
cat << EOF
# This Makefile can be used to identify the selected TARGET used for a
# specific build. It can be included by example Makefiles that need to take
# decisions based on TARGET. It is also automatically included by the
# top-level Makefile.include.

# generated automatically by tools/gen_Makefile.identify-target.sh
# msp430-support-files build ${HEADER_BUILD}  ${HEADER_DATE}
# parsed on $(date -u)

DEFAULT_TARGET=MSP430FR5994

ifneq (\$(target),)
    TARGET=\$(target)
endif

ifeq (\$(TARGET),)
    -include Makefile.target
    ifeq (\$(TARGET),)
        \${info TARGET not defined, using target '\$(DEFAULT_TARGET)'}
        TARGET=\$(DEFAULT_TARGET)
    else
        \${info using saved target '\$(TARGET)'}
    endif
endif

EOF
}

print_tail(){
    cat << EOF
else
	
    \${error unknown TARGET \$(TARGET), have a look inside \$(REFLIB_ROOT)/Makefile.identify-target for valid targets}
endif
EOF
}

special_rules()
{
    case "${1}" in
        MSP430FR5994)
            echo '	HWMULT=-mhwmult=f5series'
            echo '	CFLAGS_EXTRA=-D__MSP430_HAS_FRAM__ -DFRAM_BASE=0x0140'
            ;;
        default)
            ;;
    esac
}

expand_target()
{
    target=$1
    header=$(grep -A1 "${target}" "${GCC_EXTRA_INC_DIR}/msp430.h" | grep '#include' | sed 's|.*include[ ]"\(.*\)".*|\1|')
    unset FAMILY_N
    unset FAMILY_S
    unset FAMILY_H
    unset FAMILY
    
    local DEVICE=$(echo ${target} | sed 's|_||g')
    local DEVICE_L=$(echo ${target} | sed 's|_||g' | tr [:upper:] [:lower:])

    [ ! -e "${GCC_EXTRA_INC_DIR}/${header}" ] && echo "nu e" 
    
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

    if [ "${first_header}" == "true" ]; then
        first_header=false
    else
        echo -n "else "
    fi

cat << EOF
ifeq (\$(TARGET),${DEVICE})
	DEVICE=${DEVICE}
	DEVICE_L=${DEVICE_L}
	FAMILY=${FAMILY}
EOF
    special_rules "${DEVICE}"
    echo ''
}

while (( "$#" )); do
    if [ "$1" = "-t" ]; then
        type="${2}"
        shift; shift;
    elif [ "$1" = "-i" ]; then
        input="${2}"
        shift; shift;
    else 
        shift;
        usage
    fi
done

TARGETS=$(grep -B1 "#include.*.h" ${GCC_EXTRA_INC_DIR}/msp430.h | grep 'if defined' | sed 's|.*defined[ ](\(.*\))|\1|' | strings | grep -v GENERIC | xargs)
TARGETS_C=$(echo "${TARGETS}" | sed 's|_||g')

get_header_version

print_head

first_header=true

for target in ${TARGETS}; do
    expand_target "${target}"
done

print_tail
