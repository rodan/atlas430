#!/bin/bash

bin=("xargs" "grep" "cat" "date" "mspdebug" "lsusb" "sed" "git" "unzip" "tar" "make" "makedepend" "exuberant-ctags" "size")

package=("findutils-4.8.0" "grep-3.7" "coreutils-8.32" "coreutils-8.32" "mspdebug-0.25" "usbutils-014" "sed-4.8" "git-2.32.0" "unzip-6.0" "tar-1.34" "make-4.3" "makedepend-1.0.6" "ctags-20190331" "binutils-2.37")

for (( i=0; i<${#bin[@]}; i++ ));
do
    which "${bin[$i]}" &>/dev/null
    if [ $? != "0" ]; then
        echo " ! '${bin[$i]}' binary not found in PATH. package ${package[$i]}+ is missing"
    else
        echo "   '${bin[$i]}' - found"
    fi
done

[ ! -e /opt/reference_libs_msp430 ] && {
    echo ''
    echo ' ! "/opt/reference_libs_msp430" must point to a cloned copy'
    echo '      of the https://github.com/rodan/reference_libs_msp430 repository'
    echo ''
    echo '     pick a location (replace "sources" with any other non-temporary path) '
    echo '     for the source files and follow these steps:'
    echo ''
    echo '      $ mkdir ~/sources'
    echo '      $ cd ~/sources'
    echo '      $ git clone https://github.com/rodan/reference_libs_msp430.git'
    echo ''
    echo '     and now, as root:'
    echo ''
    echo '      # mkdir -p /opt'
    echo '      # ln -s /home/you/sources/reference_libs_msp430 /opt/reference_libs_msp430'
    echo ''
    echo "     now re-run $(basename $0), and this warning should go away"
    echo ''
}

[ ! -e /opt/msp430/bin/msp430-elf-gcc ] && {
    echo ''
    echo ' ! "/opt/msp430" must be a symlink to the latest toolchain provided on'
    echo '      the TI website: https://www.ti.com/tool/MSP430-GCC-OPENSOURCE'
    echo ''
    echo '     point your browser to the link above, select "Download options"'
    echo '     left of the MSP430-GCC-OPENSOURCE header,'
    echo '     and then download the archive behind the'
    echo '     "Mitto Systems GCC 64-bit Linux - toolchain only" link to /tmp'
    echo ''
    echo '     then proceed with the following commands, as root:'
    echo '     (making sure to update the version number)'
    echo ''
    echo '      # mkdir -p /opt'
    echo '      # tar --directory=/opt -xjf msp430-gcc-9.3.1.11_linux64.tar.bz2'
    echo '      # ln -s /opt/msp430-gcc-9.3.1.11_linux64/ /opt/msp430'
    echo ''
    echo "     now re-run $(basename $0), and this warning should go away"
    echo ''
}

[ ! -e /opt/msp430-gcc-support-files/include/ ] && {
    echo ''
    echo ' ! "/opt/msp430-gcc-support-files" must contain the latest gcc support files'
    echo '      provided on the TI website: https://www.ti.com/tool/MSP430-GCC-OPENSOURCE'
    echo ''
    echo '     point your browser to the link above, select "Download options"'
    echo '     left of the MSP430-GCC-OPENSOURCE header,'
    echo '     and then download the archive behind the'
    echo '     "Header and support files" link to /tmp'
    echo ''
    echo '     then proceed with the following commands, as root'
    echo '     (making sure to update the version number)'
    echo ''
    echo '      # mkdir -p /opt'
    echo '      # unzip -o -d /opt /tmp/msp430-gcc-support-files-1.212.zip'
    echo ''
    echo "     now re-run $(basename $0), and this warning should go away"
    echo ''
}

