SRC := src
BIN := bin

KRN_DIR   = src/kernel src/kernel/memory
KRN_SRC   = $(foreach dir,  $(KRN_DIR), $(wildcard $(dir)/*.cpp))
KRN_OBJ   = $(foreach file, $(KRN_SRC), \
				$(subst $(SRC), $(BIN), $(file:.cpp=.o)))

CC = clang++
CFLAGS = -c -ffreestanding -fno-builtin -nostdlib -O0 \
			-Isrc/ \
			-target x86_64-pc-none-elf \
			-mno-sse -mno-mmx \
			-std=c++17

$(KRN_OBJ): bin/%.o: src/%.cpp
	mkdir -p $(subst $(SRC), $(BIN), $(KRN_DIR))
	$(CC) $(CFLAGS) -o $@ $<

bin/kernel.elf: $(KRN_OBJ)
	ld.lld -T src/link.lds -o bin/kernel.elf $(KRN_OBJ)
	sudo ./fs cp bin/kernel.elf KERNEL.ELF

bin/stage2.bin: src/boot/stage2.asm
	nasm -f bin -o bin/stage2.bin src/boot/stage2.asm
	sudo ./fs cp bin/stage2.bin STAGE2.BIN

bin/stage1.bin: src/boot/stage1.asm
	nasm -f bin -o bin/stage1.bin src/boot/stage1.asm
	dd if=bin/stage1.bin of=img bs=1 count=3 conv=notrunc
	dd if=bin/stage1.bin of=img bs=1 skip=90 seek=90 conv=notrunc

run: bin/stage1.bin bin/stage2.bin bin/kernel.elf
	qemu-system-x86_64 -m 64M -monitor stdio -hdd img

clean:
	rm -rf bin/*