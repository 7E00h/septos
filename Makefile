SRC := src
BIN := bin

KRN_DIR   = src/kernel src/kernel/memory src/kernel/asm
KRN_SRC   = $(foreach dir,  $(KRN_DIR), $(wildcard $(dir)/*.cpp))
KRN_OBJ   = $(foreach file, $(KRN_SRC), \
				$(subst $(SRC), $(BIN), $(file:.cpp=.o)))

KRN_ASM_SRC = $(foreach dir, $(KRN_DIR), $(wildcard $(dir)/*.asm))
KRN_ASM_OBJ = $(foreach file, $(KRN_ASM_SRC), \
				$(subst $(SRC), $(BIN), $(file:.asm=.o)))

CC = clang++
CFLAGS = -c -ffreestanding -fno-builtin -nostdlib -O0 \
			-Isrc/ \
			-target x86_64-pc-none-elf \
			-mno-sse -mno-mmx \

$(KRN_ASM_OBJ): bin/%.o: src/%.asm
	mkdir -p $(subst $(SRC), $(BIN), $(KRN_DIR))
	nasm -f elf64 -o $@ $<

$(KRN_OBJ): bin/%.o: src/%.cpp
	mkdir -p $(subst $(SRC), $(BIN), $(KRN_DIR))
	$(CC) $(CFLAGS) -o $@ $<

bin/kernel.elf: $(KRN_OBJ) $(KRN_ASM_OBJ)
	ld.lld -T src/link.lds -o bin/kernel.elf $(KRN_OBJ) $(KRN_ASM_OBJ)
	sudo ./fs cp bin/kernel.elf KERNEL.ELF

bin/stage2.bin: src/boot/stage2.asm
	nasm -f bin -o bin/stage2.bin src/boot/stage2.asm
	sudo ./fs cp bin/stage2.bin STAGE2.BIN

bin/stage1.bin: src/boot/stage1.asm
	nasm -f bin -o bin/stage1.bin src/boot/stage1.asm
	dd if=bin/stage1.bin of=img bs=1 count=3 conv=notrunc
	dd if=bin/stage1.bin of=img bs=1 skip=90 seek=90 conv=notrunc

debug: bin/stage1.bin bin/stage2.bin bin/kernel.elf
	qemu-system-x86_64 -s -S -m 64M -monitor stdio -hdd img

run: bin/stage1.bin bin/stage2.bin bin/kernel.elf
	qemu-system-x86_64 -m 64M -monitor stdio -hdd img

clean:
	rm -rf bin/*