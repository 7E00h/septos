#!/bin/bash

_mount() {
	losetup /dev/loop15 img
	partprobe /dev/loop15
	mount -t vfat -o uid=1000,gid=1000,shortname=win95 /dev/loop15p1 mnt/
}

_umount() {
	umount mnt/
	losetup -d /dev/loop15
}

if [[ $1 == "mount" ]]; then
	_mount

elif [[ $1 == "umount" ]]; then
	_umount

elif [[ $1 == "cp" ]]; then
	_mount
	sync
	cp $2 mnt/$3
	sync
	_umount	

else
	echo "-- Usage --"
	echo "./fs <mount|umount|cp <file>>"

fi
