#include "pci.hpp"

#include <kernel/x64/cpu.hpp>
#include <kernel/memory/kmalloc.hpp>
#include <kernel/vga.hpp>

using namespace kernel;

static pci_endpoint* endpoints = nullptr;

pci_endpoint* kernel::pci_device_array()
{
    return endpoints;
}

static uint32_t pci_make_addr(uint8_t bus, uint8_t dev, uint8_t fun, uint8_t reg)
{
    uint32_t bus32 = (uint32_t) bus;
    uint32_t dev32 = (uint32_t) dev;
    uint32_t fun32 = (uint32_t) fun;
    uint32_t reg32 = (uint32_t) reg * 4;

    uint32_t addr =   (bus32 << 16)
                    | (dev32 << 11)
                    | (fun32 << 8)
                    | (reg32)
                    | (1 << 31); // Enable bit

    return addr;
}

static uint32_t pci_get_data_32(uint8_t bus, uint8_t dev, uint8_t fun, uint8_t reg)
{
    uint32_t addr = pci_make_addr(bus, dev, fun, reg);
    _asm_out_32(PCI::CONFIG_ADDR, addr);
    return _asm_in_32(PCI::CONFIG_DATA);
}

void kernel::pci_init()
{
    endpoints = (pci_endpoint*) malloc(sizeof(pci_endpoint) * 32);
    int idx   = 0;

    for (uint16_t bus = 0; bus < 256; bus++)
        for (uint8_t dev = 0; dev < 32; dev++)
        {
            uint32_t vendor32 = pci_get_data_32(bus, dev, 0, 0);
            uint16_t vendor16 = (uint16_t) (vendor32 & 0xFFFF);

            if (vendor16 == 0xFFFF)
                continue;

            pci_endpoint* cur = &endpoints[idx];

            cur->bus         = (uint8_t) bus;
            cur->device      = (uint8_t) dev;
            cur->function    = 0;
            
            cur->vendor_id   = cur->get_data_16(PCI::HDR_VEN_ID);
            cur->device_id   = cur->get_data_16(PCI::HDR_DEV_ID);
            cur->cls         = cur->get_data_8(PCI::HDR_CLSCODE);
            cur->subcls      = cur->get_data_8(PCI::HDR_SUBCLS);
            cur->hdr_type    = cur->get_data_8(PCI::HDR_TYPE);

            idx++;
        }

    // Mark last entry in array
    endpoints[idx].vendor_id = 0xFFFF;
}

uint8_t pci_endpoint::get_data_8(PCI offset)
{
    uint32_t shift = (offset % 4) * 8;
    uint32_t dword = this->get_data_32(offset);
    return (dword & (0xFF << shift)) >> shift;
}

uint16_t pci_endpoint::get_data_16(PCI offset)
{
    uint32_t shift = (offset % 2) * 16;
    uint32_t dword = this->get_data_32(offset);
    return (dword & (0xFFFF << shift)) >> shift;
}

uint32_t pci_endpoint::get_data_32(PCI offset)
{
    return pci_get_data_32(
        this->bus,
        this->device,
        this->function,
        offset / 4
    );
}