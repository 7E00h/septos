#pragma once

#include <stdint.h>
#include <stddef.h>

namespace kernel
{
    enum PCI
    {
        // IO ports
        CONFIG_DATA   = 0xCFC,
        CONFIG_ADDR   = 0xCF8,

        // Common header
        HDR_VEN_ID    = 0x00,
        HDR_DEV_ID    = 0x02,

        HDR_CMD       = 0x04,
        HDR_STATUS    = 0x06,

        HDR_REV_ID    = 0x08,
        HDR_PROGIF    = 0x09,
        HDR_SUBCLS    = 0x0A,
        HDR_CLSCODE   = 0x0B,

        HDR_CACHE     = 0x0C,
        HDR_LAT       = 0x0D,
        HDR_TYPE      = 0x0E,
        HDR_BIST      = 0x0F,

        // HDR_TYPE = 0x00
        HDR_BAR0      = 0x10,
        HDR_BAR1      = 0x14,
        HDR_BAR2      = 0x18,
        HDR_BAR3      = 0x1C,
        HDR_BAR4      = 0x20,
        HDR_BAR5      = 0x24,
        HDR_SS_VEN_ID = 0x2C,
        HDR_SS_ID     = 0x2E,
 
    };

    struct pci_endpoint
    {
    public:
        uint8_t bus;
        uint8_t device;
        uint8_t function;

        uint16_t vendor_id;
        uint16_t device_id;
        uint8_t  cls;
        uint8_t  subcls;
        uint8_t  hdr_type;

        uint8_t  get_data_8(PCI offset);
        uint16_t get_data_16(PCI offset);
        uint32_t get_data_32(PCI offset);

        void set_data_8(uint8_t data, PCI offset);
        void set_data_16(uint16_t data, PCI offset);
        void set_data_32(uint32_t data, PCI offset);
    } __attribute__((packed));

    void pci_init();
    pci_endpoint* pci_device_array();
}