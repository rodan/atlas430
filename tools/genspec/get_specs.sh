#!/bin/bash

. gen_functions.sh

usage()
{
    cat << EOF
    Usage: $0 [] [-f function] [-h]
       -f FUNCTION  filters out pin specifications for FUNCTION
       -F FAMILY    filter out by family. must be one of MSP430F5xx_6xx MSP430FR2xx_4xx MSP430FR5xx_6xx MSP430FR57xx
       -h           small usage text
EOF
    exit
}

while (( "$#" )); do
	if [ "$1" = "-f" ]; then
        filter_function="${2}"
		shift; shift;
	elif [ "$1" = "-F" ]; then
        filter_family="${2}"
		shift; shift;
    else 
		shift;
		usage
    fi
done

ucs=$(grep ifeq ../..//Makefile.identify-target | sed 's|.*,\(.*\))|\1|'  | tr '[:upper:]' '[:lower:]')

for uc in ${ucs}; do

    # try to guess the family
    FAMILY=$(guess_family_from_name "${uc}")

    [ -n "${filter_family}" ] && {
        echo "${FAMILY}" | grep -q "${filter_family}" || continue
    }

#    if [ -e "${datasheet_path}/txt_layout/${uc}.txt" ]; then
#        unset start_page
#        unset end_page

        echo -e "${GOOD}${uc}${NORMAL}  ${HILITE}${FAMILY}${NORMAL}"
#        toc=$(grep --after-context=20 --extended-regexp 'Input/Output Diagrams[ \.]+' "${datasheet_path}/txt_layout/${uc}.txt")

        [ ! -e "${DATASHEET_PATH}/txt_layout/${uc}.txt" ] && echo " no datasheet found"

        #start_page=$(grep 'Input/Output Diagrams[ \.]*' "${datasheet_path}/txt_nolayout/${uc}.txt" | sed 's|.*Input/Output Diagrams[ \.]*\([0-9]*\).*|\1|')
        #start_chapter=$(grep 'Input/Output Diagrams[ \.]*' "${datasheet_path}/txt_nolayout/${uc}.txt" ) # | grep -o '^[0-9\.]*') # sed 's|^\([0-9\.]*\)*[ ]Input/Output Diagrams.*|\1|')
        #[ ! -z "${start_page}" ] && {
        #    end_page=$(grep -A1 'Input/Output Diagrams[ \.]*[0-9]' "${datasheet_path}/txt_nolayout/${uc}.txt" | tail -n1 | grep -o '[0-9]*$')
        #    if [ -z ${end_page} ]; then
        #        end_page=$(grep -A2 'Input/Output Diagrams[ \.]*[0-9]' "${datasheet_path}/${uc}.txt" | tail -n1 | grep -o '[0-9]*$')
        #    else 
        #        [ "${end_page}" -lt "${start_page}" ] && {
        #            echo "inverse endpage 1 <------------------------------------------"
        #        }
        #    fi
        #}
        #[ -n "${start_page}" -a -n "${end_page}" -a "${end_page}" -lt "${start_page}" ] && {
        #    echo "bad end page #####################"
        #    unset end_page
        #}
#        echo "'$toc'"
        #echo "'${start_page}' '${start_chapter}' .. '${end_page}'"
        #grep -A10 'Input/Output Diagrams[ \.]*[0-9]' "${datasheet_path}/txt_nolayout/${uc}.txt" # | tail -n1 # | grep -o '[0-9]*$'
#    fi

done
