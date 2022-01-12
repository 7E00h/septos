/*
    elf.hpp

    Only supports 64 bit ELF files
*/

#pragma once

#include <kernel/utility/utility.hpp>

#define EI_MAG0       0
#define EI_MAG1       1
#define EI_MAG2       2
#define EI_MAG3       3
#define EI_CLASS      4
#define EI_DATA       5
#define EI_VERSION    6
#define EI_OSABI      7
#define EI_ABIVERSION 8
#define EI_PAD        9

#define PT_LOAD 1

namespace kernel
{
    struct elf_hdr
    {
        uint8_t  e_ident[16]; // Contains information about the ELF image
        uint16_t e_type;      // Object file type
        uint8_t  e_machine;   // Target ISA
        uint32_t e_version;   // 1 == Original Version
        uint64_t e_entry;     // Program entry point
        uint64_t e_phoff;     // Program header offset
        uint64_t e_shoff;     // Section header offset
        uint32_t e_flags;     // Depends on target architecture
        uint16_t e_ehsize;    // Size of this header
        uint16_t e_phentsize; // Size of program header table entry
        uint16_t e_phnum;     // # of entries in program header table
        uint16_t e_shentsize; // Size of section header table entry
        uint16_t e_shnum;     // # of entries in section header table
        uint16_t e_shstrndx;  // Index of the string table
    };

    struct elf_program_hdr
    {
        uint32_t p_type;
        uint32_t p_flags;
        uint64_t p_offset;
        uint64_t p_vaddr;
        uint64_t p_paddr;
        uint64_t p_filesz;
        uint64_t p_memsz;
        uint64_t p_align;
    };

    struct elf_section_hdr
    {
        uint32_t sh_name;
        uint32_t sh_type;
        uint64_t sh_flags;
        uint64_t sh_addr;
        uint64_t sh_offset;
        uint64_t sh_size;
        uint32_t sh_link;
        uint32_t sh_info;
        uint64_t sh_addralign;
        uint64_t sh_entsize;
    };

    struct elf_img
    {
        elf_hdr*         hdr;
        elf_program_hdr* program_hdr;
        elf_section_hdr* section_hdr;
    };

    bool parse_elf_hdr(elf_hdr* hdr, elf_img* out);
}