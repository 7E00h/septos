OBJECTS = \
	kernel

bin/kernel.elf : bin/$(OBJECTS).o
	ld.lld -T src/link.lds -o bin/kernel.elf $<
	sudo ./fs cp bin/kernel.elf KERNEL.ELF

bin/$(OBJECTS).o : src/$(OBJECTS).c
	clang -c -ffreestanding -fno-builtin -nostdlib -nostdinc -O0 \
		-target x86_64-pc-none-elf \
		-o $@ $<

bin/stage2.bin : src/boot/stage2.asm
	nasm -f bin -o bin/stage2.bin src/boot/stage2.asm
	sudo ./fs cp bin/stage2.bin STAGE2.BIN

bin/stage1.bin : src/boot/stage1.asm
	nasm -f bin -o bin/stage1.bin src/boot/stage1.asm
	dd if=bin/stage1.bin of=img bs=1 count=3 conv=notrunc
	dd if=bin/stage1.bin of=img bs=1 skip=90 seek=90 conv=notrunc

run : bin/stage1.bin bin/stage2.bin bin/kernel.elf
	qemu-system-x86_64 -m 64M -monitor stdio -hdd img

clean :
	rm bin/*