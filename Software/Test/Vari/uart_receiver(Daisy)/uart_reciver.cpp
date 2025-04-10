#include "daisy_seed.h"
// #include "daisysp.h"
#include "pentaPianta.h"
#include "Effect.h"
using namespace daisy;
// using namespace daisysp;

UartHandler uart;
DaisySeed hw;
pentaPianta penta[3]; // Array of pentaPianta objects
Effect effect[4]; // Array of Effect objects

uint8_t start_bit = 'S';
void getData()
{
	uint8_t data[3];
	uint8_t start_bit_1 = 'S';
	while (start_bit_1 != 'B') // Wait for the start bit
	{
		uart.BlockingReceive(&start_bit_1, 1, 1000);
		hw.PrintLine("waiting B \n");
		hw.PrintLine("start bit %d \n", start_bit_1);
	}
	hw.PrintLine("Il test è iniziato \n");
	for (size_t i = 0; i < 3; i++)
	{
		 // Update the pentaPianta object i
		uart.BlockingReceive(data, 3, 100); // Receive 3 bytes of data with a timeout of 1 second
		if (data[0] == 0)
		{
			penta[i].zeroCount(); // Set the first parameter of pentaPianta object i
		}
		else
		{
			penta[i].incCount();					  // Increment the count of the number of times the sensor has been read
		}
		penta[i].setRawValue(((uint16_t)data[2]<<8)+(uint16_t)data[1]);

	}
		for (size_t i = 0; i < 4; i++)
	{
		uart.BlockingReceive(data, 3, 100); // Receive 3 bytes of data with a timeout of 1 second
		if (data[0] == 0)
		{
			effect[i].setGate(false); // Set the first parameter of pentaPianta object i
		}
		else
		{
			effect[i].setGate(true);					  // Increment the count of the number of times the sensor has been read
		}
		penta[i].setRawValue(((uint16_t)data[2]<<8)+(uint16_t)data[1]);

	}
}

int main(void)
{
	int counter=3;
	hw.Configure();
    hw.Init();
    hw.StartLog(false);
    System::Delay(3000);
    hw.PrintLine("Starting Read-test");
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
	hw.PrintLine("Starting Read-test 2");

	while (start_bit != 'B' || counter != 0) // Wait for the start bit
	{
		uart.BlockingReceive(&start_bit, 1, 10000);
		if(start_bit == 'B')
		{
			counter--;
		}else
		{
			counter = 3;
		}
		hw.PrintLine("waiting I \n");
		hw.PrintLine("start bit %d \n", start_bit);
		hw.PrintLine("counter is %d \n", counter);
	}

	while (1)
	{
		getData(); // Call the function to get data from the UART
		for (size_t i = 0; i < 3; i++)
		{
			hw.PrintLine("Il valore della pianta è: %d\n",penta[i].getRawValue()); // Update the pentaPianta object i
			hw.PrintLine("Il valore del gate è %d \n",penta[i].getGate());  // Print the pentaPianta object i
		}
	}
}
