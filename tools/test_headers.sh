#!/bin/bash

# script that checks all fram-enabled uCs for __MSP430_HAS_FRAM__
# and reports headers that failed this criteria
#
# source:    https://github.com/rodan/atlas430
# author:    Petre Rodan <2b4eda@subdimension.ro>
# license:   BSD


# you might need to tweak these 3 locations
CC='/opt/msp430/bin/msp430-elf-gcc'
SUPPORT_FILES='/opt/msp430-gcc-support-files/include'
TEMP_DIR='/tmp/build'

list_of_ucs=$(ls -1 ${SUPPORT_FILES}/*430fr*.h | sed 's|\.h$||;s|.*include/||' | grep -Ev '(generic)|(rom)' | tr '[:lower:]' '[:upper:]')

create_main()
{
    mkdir -p "${TEMP_DIR}"
    cat << EOF > "${TEMP_DIR}/main.c"
#include <msp430.h>

int main(void)
{
#ifndef __MSP430_HAS_FRAM__
    #error MACRO_IS_MISSING
#endif
}
EOF
}

compile_target()
{
    target=$1
    target_lowercase=$(echo "${target}" | tr '[:upper:]' '[:lower:]')
    build_dir="${TEMP_DIR}/${target}"
    mkdir -p "${build_dir}"
    if ${CC} -I ${SUPPORT_FILES} -I -D__${target}__ -mmcu=${target} -Wall -Werror -c ${TEMP_DIR}/main.c -o "${build_dir}/main.o"; then
        #${CC} -T ${SUPPORT_FILES}/${target_lowercase}.ld -L /opt/msp430/msp430-elf/lib -L /opt/msp430-gcc-support-files/include -mmcu=${target} -g -Wl,--gc-sections ${build_dir}/main.o -o ${build_dir}/main.elf
        echo "${target} is OK"
        return 0
    else
        echo "${target} is broken"
        return 1
    fi
}

create_main

for target in ${list_of_ucs}; do
    compile_target "${target}" || broken_list="${broken_list} ${target}"
done

echo "list of suspect headers: ${broken_list}"
