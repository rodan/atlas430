#!/bin/bash

#set -e

. gen_functions.sh

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
        MSP430FR2355)
            echo '  MEMORY_MODEL=LARGE'
            ;;
        default)
            ;;
    esac
}

expand_target()
{
    target=$1

    local E_DEVICE
    local E_DEVICE_L
    local E_FAMILY
    
    E_DEVICE="${target//_/}"
    E_DEVICE_L=$(echo "${target//_/}" | tr '[:upper:]' '[:lower:]')
    E_FAMILY=$(guess_family_from_target "${target}")

    [ -z "${E_FAMILY}" ] && return 1

    if [ "${first_header}" == "true" ]; then
        first_header=false
    else
        echo -n "else "
    fi

cat << EOF
ifeq (\$(TARGET),${E_DEVICE})
	DEVICE=${E_DEVICE}
	DEVICE_L=${E_DEVICE_L}
	FAMILY=${E_FAMILY}
EOF
    special_rules "${E_DEVICE}"
    echo ''
}

TARGETS=$(grep -B1 "#include.*.h" ${GCC_EXTRA_INC_DIR}/msp430.h | grep 'if defined' | sed 's|.*defined[ ](\(.*\))|\1|' | strings | grep -v GENERIC | sort | xargs)

get_header_version

print_head

first_header=true

for target in ${TARGETS}; do
    grep -q "${target}" target_blacklist.txt && {
        err "target ${target} has been skipped"
        continue
    }
    expand_target "${target}" || err "family not found for target ${target}"
done

print_tail
