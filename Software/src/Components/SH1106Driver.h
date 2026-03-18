#pragma once
#include "daisy_seed.h"

// ---------------------------------------------------------------------------
// I2C Transport for SH1106 — uses an external (shared) I2CHandle
// ---------------------------------------------------------------------------
class SH1106I2CTransport
{
  public:
    struct Config
    {
        daisy::I2CHandle* i2c;
        uint8_t           i2c_address;
        uint32_t          timeout_ms;
        uint8_t           max_chunk_size;

        Config()
        : i2c(nullptr), i2c_address(0x3C), timeout_ms(1000), max_chunk_size(16)
        {
        }
    };

    void Init(const Config& config)
    {
        i2c_            = config.i2c;
        i2c_address_    = config.i2c_address;
        timeout_ms_     = config.timeout_ms;
        max_chunk_size_ = config.max_chunk_size == 0 ? 1 : config.max_chunk_size;
        if(max_chunk_size_ > 128)
            max_chunk_size_ = 128;
    }

    void SendCommand(uint8_t cmd)
    {
        if(i2c_ == nullptr)
            return;
        uint8_t buf[2] = {0x00, cmd};
        i2c_->TransmitBlocking(i2c_address_, buf, 2, timeout_ms_);
    }

    void SendData(uint8_t* buff, size_t size)
    {
        if(i2c_ == nullptr)
            return;
        // Send in chunks: [0x40, d0, d1, ..., dN]
        // Keep chunk size conservative for better compatibility with SH1106 clones.
        uint8_t txbuf[129];
        txbuf[0] = 0x40;
        while(size > 0)
        {
            size_t n = (size > max_chunk_size_) ? max_chunk_size_ : size;
            for(size_t i = 0; i < n; i++)
                txbuf[i + 1] = buff[i];
            i2c_->TransmitBlocking(i2c_address_, txbuf, n + 1, timeout_ms_);
            buff += n;
            size -= n;
        }
    }

  private:
    daisy::I2CHandle* i2c_;
    uint8_t           i2c_address_;
        uint32_t          timeout_ms_;
        uint8_t           max_chunk_size_;
};

// ---------------------------------------------------------------------------
// SH1106 Driver — page-addressed OLED with 2-column offset
//
// Key differences from SSD1306:
//   - No horizontal addressing mode (page mode only)
//   - 132-column internal RAM, display starts at column 2
//   - Different charge pump command (0xAD/0x8B vs 0x8D/0x14)
// ---------------------------------------------------------------------------
template <size_t width, size_t height, typename Transport>
class SH1106Driver
{
  public:
    struct Config
    {
        typename Transport::Config transport_config;
                uint8_t                    column_offset;

                Config() : column_offset(2) {}
    };

    void Init(Config config)
    {
        transport_.Init(config.transport_config);
                column_offset_ = config.column_offset;

        transport_.SendCommand(0xAE); // Display off

        transport_.SendCommand(0xD5); // Clock divide / oscillator
        transport_.SendCommand(0x80);

        transport_.SendCommand(0xA8); // Multiplex ratio
        transport_.SendCommand(height - 1);

        transport_.SendCommand(0xD3); // Display offset
        transport_.SendCommand(0x00);

        transport_.SendCommand(0x40); // Start line = 0

        // SH1106 internal DC-DC charge pump
        transport_.SendCommand(0xAD);
        transport_.SendCommand(0x8B);

        transport_.SendCommand(0xA1); // Segment remap (col 127 → SEG0)
        transport_.SendCommand(0xC8); // COM scan direction (remapped)

        transport_.SendCommand(0xDA); // COM pins config
        transport_.SendCommand(height == 64 ? 0x12 : 0x02);

        transport_.SendCommand(0x81); // Contrast
        transport_.SendCommand(0xFF);

        transport_.SendCommand(0xD9); // Pre-charge period
        transport_.SendCommand(0x1F);

        transport_.SendCommand(0xDB); // VCOMH deselect
        transport_.SendCommand(0x40);

        transport_.SendCommand(0xA4); // Display follows RAM
        transport_.SendCommand(0xA6); // Normal (not inverted)

        Fill(false);
        Update();

        transport_.SendCommand(0xAF); // Display on
    }

    size_t Width() const { return width; }
    size_t Height() const { return height; }

    void DrawPixel(uint_fast8_t x, uint_fast8_t y, bool on)
    {
        if(x >= width || y >= height)
            return;
        if(on)
            buffer_[x + (y / 8) * width] |= (1 << (y % 8));
        else
            buffer_[x + (y / 8) * width] &= ~(1 << (y % 8));
    }

    void Fill(bool on)
    {
        for(size_t i = 0; i < sizeof(buffer_); i++)
            buffer_[i] = on ? 0xFF : 0x00;
    }

    void Update()
    {
        for(uint8_t page = 0; page < (height / 8); page++)
        {
            uint8_t col = column_offset_;
            transport_.SendCommand(0xB0 + page); // Page address
            transport_.SendCommand(0x00 | (col & 0x0F));
            transport_.SendCommand(0x10 | ((col >> 4) & 0x0F));
            transport_.SendData(&buffer_[width * page], width);
        }
    }

  private:
    Transport transport_;
    uint8_t   column_offset_;
    uint8_t   buffer_[width * height / 8];
};

// Convenient type alias
using SH1106I2c128x64Driver = SH1106Driver<128, 64, SH1106I2CTransport>;
