#!/bin/bash

export BASEDIR=$(pwd)
export MAKEOBJDIRPREFIX=$BASEDIR/obj
cd $BASEDIR/src

world() {
    make buildworld TARGET_ARCH=amd64 -j4
}

kernel() {
    make buildkernel TARGET_ARCH=amd64 KERNCONF=GENERIC -j4
}

install() {
    make installkernel TARGET_ARCH=amd64 KERNCONF=GENERIC DESTDIR=$BASEDIR/kernel
}

case "$1" in
	world)        
		world
		;;
	kernel)
		kernel
		;;
	install)
		install
		;;
	*)
		echo "Usage: ./build.sh world | kernel | install" 
		exit
		;;
esac