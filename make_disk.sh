sudo dd if=/dev/zero of=img bs=1M count=1024
cfdisk img
sudo losetup /dev/loop15 img
sudo partprobe /dev/loop15
sudo mkfs.vfat -F32 /dev/loop15p1
sudo losetup -d /dev/loop15
