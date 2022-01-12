#include "elf.hpp"

using namespace kernel;

#define BYTE_ADD(a, b) (((uint64_t) a) + b)

static bool _is_valid(elf_hdr* hdr)
{
    return hdr->e_ident[EI_MAG0]  == 0x7F
        && hdr->e_ident[EI_MAG1]  == 'E'
        && hdr->e_ident[EI_MAG2]  == 'L'
        && hdr->e_ident[EI_MAG3]  == 'F'
        && hdr->e_ident[EI_CLASS] == 2;
}

bool kernel::parse_elf_hdr(elf_hdr* hdr, elf_img* out)
{
    if (!_is_valid(hdr))
        return false;

    out->hdr         = hdr;
    out->program_hdr = (elf_program_hdr*) BYTE_ADD(hdr, hdr->e_phoff);
    out->section_hdr = (elf_section_hdr*) BYTE_ADD(hdr, hdr->e_shoff);

    return true;
}