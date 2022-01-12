OBJ = $(subst .cpp,.o,$(wildcard *.cpp))

$(OBJ):
	clang++                        \
		-c                         \
		-ffreestanding             \
		-fno-builtin               \
		-nostdlib                  \
		-O0                        \
		-target x86_64-pc-none-elf \
		-mno-sse -mno-mmx          \
		-fno-rtti -fno-exceptions  \
		-mcmodel=large             \
		-o $@ $(subst .o,.cpp,$@)

	ld -T septos.lds -o $(subst .o,.elf,$@) $@
	xxd -i $(subst .o,.elf,$@) >> $(subst .o,.inc,$@)

all: $(OBJ)
	rm *.o

clean:
	rm *.inc *.elf
