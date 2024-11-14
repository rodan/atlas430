#!/bin/sh

#
# NAME
#       minicom_wrapper.sh - a wrapper for the net-dialup/minicom application that automatically connects to a MSP430 tty-over usb channel if one is detected
#
# SYNOPSIS
#       minicom_wrapper.sh
#
# DESCRIPTION
#       a script that searches for tty-over usb channels and checks if the device is already in use before starting minicom
#       
# AUTHOR
#       Petre Rodan, 2024
#

default_baudrate='57600'
version='2024 Nov 14'

usage() {
    echo "wrapper for minicom, version ${version}"
    echo "Usage: $0 [-b NUM] [filter]"
    echo "    -b NUM   set baudrate to NUM, deafult is ${default_baudrate}"
    echo "    filter   use this string as name/usb device id/ugen to select one device"
}

while [ "$#" != 0 ]; do
    if [ "$1" = "-b" ]; then
        shift;
        default_baudrate=$1
        shift;
    elif [ "$1" = "-h" ]; then
        usage
        exit 0
    else
        filter="${1}"
        shift;
    fi
done

detected=$(lsusb | grep -E '(Texas Instruments MSP)|(Future Technology Devices International)')
preference=$(echo "${detected}" | grep -i "${filter}")
preference_lc=$(echo "${preference}" | wc -l)

[ -z "${preference}" ] && {
    echo '  error: no compatible usb device has been detected, exiting'
    exit 1
}

echo '  detected devices:'
echo "${detected}"

[ "${preference_lc}" -gt 1 ] && {
    echo '  error: multiple devices detected, use a filter to pick only one'
    echo ''
    usage
    exit 1
}

echo '  current setup:'
echo "    default_baudrate ${default_baudrate}"
echo "    filter '${filter}'"

check_if_in_use() {
    dev=$1
    in_use=$(lsof 2>/dev/null | grep "${dev}")
    [ -n "${in_use}" ] && {
        echo "  error: device ${dev} is already used"
        pname=$(echo "${in_use}" | awk '{ print $1 }' | xargs)
        pid=$(echo "${in_use}" | awk '{ print $2 }' | xargs)
        echo "  by process ${pname}, pid ${pid}"
        return 1
    }
    return 0
}

run_minicom_FreeBSD() {
    ugen=$(echo "${preference}" | sed 's|.*\/dev/\(ugen[0-9]*.[0-9]*\).*|\1|')
    echo "    ugen ${ugen}"
    [ -z "${ugen}" ] && return 1

    interface=$(usbconfig -v -d /dev/"${ugen}" | grep -B 20 -E '(MSP Application UART)|(TTL232RG)' | grep 'Interface\ [0-9]*$' | sed 's|.*Interface\ \([0-9]*\)$|\1|')
    echo "    interface ${interface}"
    [ -z "${interface}" ] && return 1

    # get device id
    device_id=$(sysctl -a | grep "^dev\..*ugen=${ugen}" | sed 's|^\(dev\.[a-zA-Z]*\.[0-9]*\)\..*|\1|' | head -n1)
    echo "    device_id ${device_id}"

    ttyname=$(sysctl -n "${device_id}.ttyname")
    echo "    ttyname ${ttyname}"
    [ -z "${ttyname}" ] && return 1

    check_if_in_use "/dev/tty${ttyname}" || return 1

    /usr/local/bin/minicom --device "/dev/tty${ttyname}" --baudrate "${default_baudrate}" --color on
}

run_minicom_Linux() {
    ifaces=$(ls -1 /dev/ttyACM* /dev/ttyUSB*)
    usb_vendor=$(echo "${preference}" | awk '{ print $6 }' | cut -d':' -f1 )
    usb_model=$(echo "${preference}" | awk '{ print $6 }' | cut -d':' -f2 )
    usb_interface_num=$(lsusb -v -d "${usb_vendor}:${usb_model}" | grep -B10 'MSP Application UART' | grep 'bInterfaceNumber' | awk '{ print $2 }' | head -n1)
    echo "    usb_id '${usb_vendor}:${usb_model}' interface '${usb_interface_num}'"

    for if in ${ifaces}; do
        if_specs=$(udevadm info --name="${if}")
        echo "${if_specs}" | grep -q "ID_VENDOR_ID=${usb_vendor}" && 
            echo "${if_specs}" | grep -q "ID_MODEL_ID=${usb_model}" &&
            echo "${if_specs}" | grep -q "ID_USB_INTERFACE_NUM=0${usb_interface_num}" && {
            ttyname="${if}"
            break
        }
    done

    [ -z "${ttyname}" ] && return 1

    echo "    found_device ${ttyname}"
    check_if_in_use "${ttyname}" || return 1

    /usr/bin/minicom --device "${ttyname}" --baudrate "${default_baudrate}" --color on
}

os=$(uname -s)
if [ "${os}" = "FreeBSD" ]; then
    run_minicom_FreeBSD
elif [ "$os" = "Linux" ]; then
    run_minicom_Linux
else
    echo "  error: unsupported OS, exiting"
fi


