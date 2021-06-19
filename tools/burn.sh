#!/bin/bash

[ -e "$1" ] && hex="$1" || {
    echo 'binary file not provided, exiting'
    exit 1
}

lsusb | grep -qi '0403:6001' && prog='ftdi'
lsusb | grep -qi '2047:0013' && prog='ezfet-1.2'
lsusb | grep -qi '2047:0014' && prog='msp-fet'
lsusb | grep -qi 'Dragon' && prog='dragon'
lsusb | grep -qi 'PL2303' && prog='stk500v2'
lsusb | grep -qi 'eZ430 Development Tool' && prog='ez430'
lsusb | grep -qi 'BlueRobin RF' && prog='bluerobin'
lsusb | grep -qi '15ba:0100' && prog='olimex_iso_mk2'

[ -z "${prog}" ] && {
    echo 'programmer not found'
    exit 1
}

[ "${prog}" = "stk500v2" ] && {
    for serial in USB0 USB1 USB2 USB3; do
        grep -q 'pl2303' /sys/class/tty/tty${serial}/device/uevent 2>/dev/null && dev=/dev/tty${serial}
    done
    echo "+ detected $prog"
    set -x
    avrdude -C/usr/avr/etc/avrdude.conf -patmega328p -cstk500v2 -P${dev} -e -Ulfuse:w:0xff:m -Uhfuse:w:0xde:m -Uefuse:w:0x05:m -Ulock:w:0x3F:m -Uflash:w:${hex}:i
    exit $?
}

[ "${prog}" = "dragon" ] && {
    echo "+ detected $prog"
    set -x
    avrdude -C/usr/avr/etc/avrdude.conf -patmega328p -cdragon_isp -Pusb -b 115200 -e -Ulfuse:w:0xff:m -Uhfuse:w:0xde:m -Uefuse:w:0x05:m -Ulock:w:0x3F:m -Uflash:w:${hex}:i
    exit $?
}

[ "${prog}" = "ftdi" ] && {
    for serial in USB0 USB1 USB2 USB3; do
        grep -q 'ftdi_sio' /sys/class/tty/tty${serial}/device/uevent 2>/dev/null && dev=/dev/tty${serial}
    done
    echo "+ detected $prog"
    set -x
    avrdude -C/usr/avr/etc/avrdude.conf -patmega328p -carduino -P${dev} -b57600 -D -Uflash:w:${hex}:i
    exit $?
}

[ "${prog}" = "ezfet-1.2" ] && {
    echo "+ detected $prog"
    set -x
    mspdebug --allow-fw-update tilib "prog ${hex}"
    exit $?
}

[ "${prog}" = "msp-fet" ] && {
    echo "+ detected $prog"
    set -x
    mspdebug --allow-fw-update tilib "prog ${hex}"
    exit $?
}

[ "${prog}" = "ez430" ] && {
    echo "+ detected $prog"
    set -x
    mspdebug rf2500 "prog ${hex}"
    exit $?
}

[ "${prog}" = "olimex_iso_mk2" ] && {
    echo "+ detected $prog"
    set -x
    mspdebug olimex-iso-mk2 "prog ${hex}"
    exit $?
}

