#!/bin/bash

_mount() {
	losetup /dev/loop0 img
	mount -t vfat -o uid=1000,gid=1000,shortname=win95 /dev/loop0 mnt/
}

_umount() {
	umount mnt/
	losetup -d /dev/loop0
}

if [[ $1 == "mount" ]]; then
	_mount

elif [[ $1 == "umount" ]]; then
	_umount

elif [[ $1 == "cp" ]]; then
	_mount
	cp $2 mnt/$3
	_umount	

else
	echo "-- Usage --"
	echo "./fs <mount|umount|cp <file>>"

fi
