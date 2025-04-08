#include "daisy_seed.h"
using namespace daisy;
DaisySeed hw;
int main(void)
{
    hw.Configure();
    hw.Init();
    hw.StartLog(false);
    System::Delay(3000);
    hw.PrintLine("Starting Read-test");
    UartHandler         uart;
    UartHandler::Config config;
    config.baudrate = 9600 ;
     config.periph   = UartHandler::Config::Peripheral::USART_1;
    config.stopbits      = UartHandler::Config::StopBits::BITS_1;
    config.parity        = UartHandler::Config::Parity::NONE;
    config.mode          = UartHandler::Config::Mode::TX_RX;
    config.wordlength    = UartHandler::Config::WordLength::BITS_8;
    config.pin_config.rx = {DSY_GPIOB, 7};  // (USART_1 RX) Daisy pin 15
    config.pin_config.tx = {DSY_GPIOB, 6};  // (USART_1 TX) Daisy pin 14
    
    uart.Init(config);
    //uart.StartRx();

    for(;;)
    {
        uint8_t mybuffer;
       int ret = uart.PollReceive(&mybuffer, 1, 1);
        if(ret == 0) {
            hw.PrintLine("%c", mybuffer);
        }
    }
}