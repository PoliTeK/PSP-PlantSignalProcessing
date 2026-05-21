#ifndef FLASH_UTILS_H
#define FLASH_UTILS_H

#include "daisy_seed.h"
#include <cstring> 

namespace FlashUtils {

    template <typename T>
    bool write(uint32_t start_addr, const T& data, daisy::DaisySeed& hw) {
        size_t size = sizeof(T);
        
        // Cannot write without first erasing
        if(hw.qspi.Erase(start_addr, start_addr + size) != daisy::QSPIHandle::Result::OK) {
            return false;
        }

        if(hw.qspi.Write(start_addr, size, (uint8_t*)&data) != daisy::QSPIHandle::Result::OK) {
            return false;
        }
        
        return true;
    }

    template <typename T>
    void read(uint32_t start_addr, T& data) {
        size_t size = sizeof(T);
        uint8_t* flash_ptr = (uint8_t*)(0x90000000 + start_addr); // 0x90000000 = starting address of the flash memory

        dsy_dma_invalidate_cache_for_buffer(flash_ptr, size); // invalidate cache to get real data
        std::memcpy(&data, flash_ptr, size);
    }
}

#endif