#pragma once

#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"

namespace daisy {

class SSD1306I2cDmaTransport {
public:
    struct Config {
        Config() { Defaults(); }
        I2CHandle::Config i2c_config;
        uint8_t i2c_address;
        void Defaults() {
            i2c_config.periph = I2CHandle::Config::Peripheral::I2C_1;
            i2c_config.speed = I2CHandle::Config::Speed::I2C_1MHZ;
            i2c_config.mode = I2CHandle::Config::Mode::I2C_MASTER;
            i2c_config.pin_config.scl = daisy::Pin(PORTB, 8);
            i2c_config.pin_config.sda = daisy::Pin(PORTB, 9);
            i2c_address = 0x3C;
        }
    };

    void Init(const Config& config) {
        _i2c_address_ = config.i2c_address;
        i2c_.Init(config.i2c_config);
    }

    void SendCommand(uint8_t cmd) {
        uint8_t buf[2] = {0x00, cmd};
        i2c_.TransmitBlocking(_i2c_address_, buf, 2, 100);
    }

    void SendCommands(uint8_t* cmds, size_t size) {
        for(size_t i = 0; i < size; i++) {
            SendCommand(cmds[i]);
        }
    }

    void SendDataDma(uint8_t* buff, size_t size, I2CHandle::CallbackFunctionPtr callback, void* context) {
        // 1. Prepara il pacchetto unendo il byte di comando (0x40) e i byte grafici
        _dma_buffer_[0] = 0x40;
        memcpy(&_dma_buffer_[1], buff, size);
        
        // 2. Forza la CPU a scrivere le modifiche (appena fatte col memcpy) nella RAM fisica
        // In questo modo il DMA hardware non leggerà dati obsoleti.
        dsy_dma_clear_cache_for_buffer(_dma_buffer_, size + 1);
        
        // 3. Avvia il trasferimento hardware in background
        i2c_.TransmitDma(_i2c_address_, _dma_buffer_, size + 1, callback, context);
    }

private:
    daisy::I2CHandle i2c_;
    uint8_t _i2c_address_;
    
    // Alloca 1025 byte nella zona di memoria D2, ottimizzata per il DMA
    uint8_t _dma_buffer_[1025];
};


// --- PARTE 2: Il Driver (Gestione Grafica) ---
template <size_t width, size_t height>
class SSD1306DmaDriver {
public:
    struct Config {
        typename SSD1306I2cDmaTransport::Config transport_config;
    };

    void Init(Config config) {
        transport_.Init(config.transport_config);

        // Sequenza di inizializzazione standard SSD1306
        transport_.SendCommand(0xAE); // Display Off
        transport_.SendCommand(0xD5); transport_.SendCommand(0x80); // Clock Divide
        transport_.SendCommand(0xA8); transport_.SendCommand(0x3F); // Multiplex Ratio (64 pixel)
        transport_.SendCommand(0xD3); transport_.SendCommand(0x00); // Display Offset
        transport_.SendCommand(0x40); // Start Line
        transport_.SendCommand(0x8D); transport_.SendCommand(0x14); // Charge Pump
        
        // COMANDO CHIAVE: Imposta l'Addressing Orizzontale
        transport_.SendCommand(0x20); transport_.SendCommand(0x00); 
        
        transport_.SendCommand(0xA1); // Segment Remap
        transport_.SendCommand(0xC8); // COM Direction
        transport_.SendCommand(0xDA); transport_.SendCommand(0x12); // COM Pins
        transport_.SendCommand(0x81); transport_.SendCommand(0xCF); // Contrast
        transport_.SendCommand(0xD9); transport_.SendCommand(0xF1); // Pre-charge
        transport_.SendCommand(0xDB); transport_.SendCommand(0x40); // VCOM Detect
        transport_.SendCommand(0xA4); // All On Resume
        transport_.SendCommand(0xA6); // Normal Display
        transport_.SendCommand(0xAF); // Display On
    }

    void DrawPixel(uint_fast8_t x, uint_fast8_t y, bool on) {
        if(x >= width || y >= height) return;
        if(on) buffer_[x + (y / 8) * width] |= (1 << (y % 8));
        else   buffer_[x + (y / 8) * width] &= ~(1 << (y % 8));
    }

    void Fill(bool on) {
        memset(buffer_, on ? 0xFF : 0x00, sizeof(buffer_));
    }

    size_t Width() const { return width; }
    size_t Height() const { return height; }
    bool UpdateFinished() { return !updating_; }

    void Update() {
        if (!updating_) {
            updating_ = true;
            
            // Riposiziona il cursore dell'OLED in alto a sinistra (Colonna 0, Pagina 0)
            uint8_t reset_cmds[] = {0x21, 0, 127, 0x22, 0, 7}; 
            transport_.SendCommands(reset_cmds, 6);
            
            transport_.SendDataDma(buffer_, sizeof(buffer_), DmaCallback, this);
        }
    }

private:
    SSD1306I2cDmaTransport transport_;
    uint8_t buffer_[width * height / 8];
    volatile bool updating_ = false;

    // Questa funzione viene chiamata automaticamente dall'hardware quando il DMA ha finito
    static void DmaCallback(void* context, daisy::I2CHandle::Result result) {
        static_cast<SSD1306DmaDriver*>(context)->updating_ = false;
    }
};

using SSD1306I2cDma128x64Driver = SSD1306DmaDriver<128, 64>;
using MyDmaOledDisplay = OledDisplay<SSD1306I2cDma128x64Driver>;

} // namespace daisy