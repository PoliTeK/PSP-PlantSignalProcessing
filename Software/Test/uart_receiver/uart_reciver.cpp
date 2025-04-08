#include "daisy_seed.h"
// #include "daisysp.h"
#include "pentaPianta.h"
#include "Effect.h"
using namespace daisy;
// using namespace daisysp;

UartHandler uart;
UartHandler::Config config;
DaisySeed hw;
pentaPianta penta[3]; // Array of pentaPianta objects
Effect effect[4]; // Array of Effect objects
void getData()
{
	char start_bit = 'S';
	uint8_t data[3];
	while (start_bit != 'B') // Wait for the start bit
	{
		uart.BlockingReceive((uint8_t*)&start_bit, sizeof(start_bit), 100);
	}
	for (size_t i = 0; i < 3; i++)
	{
		uart.BlockingReceive(data, 3, 100); // Receive 3 bytes of data with a timeout of 1 second
		if (data[0] == 0)
		{
			penta[i].zeroCount(); // Set the first parameter of pentaPianta object i
			penta[i].setRawValue(((uint16_t)data[1]<<8)+(uint16_t)data[2]);
		}
		else
		{
			penta[i].incCount();					  // Increment the count of the number of times the sensor has been read
			penta[i].setRawValue(((uint16_t)data[1]<<8)+(uint16_t)data[2]); // Set the second parameter of pentaPianta object i
		}
	}
		for (size_t i = 0; i < 4; i++)
	{
		uart.BlockingReceive(data, 3, 100); // Receive 3 bytes of data with a timeout of 1 second
		if (data[0] == 0)
		{
			effect[i].setGate(false); // Set the first parameter of pentaPianta object i
			effect[i].setRawValue(((uint16_t)data[1]<<8)+(uint16_t)data[2]);
		}
		else
		{
			effect[i].setGate(true);					  // Increment the count of the number of times the sensor has been read
			effect[i].setRawValue(((uint16_t)data[1]<<8)+(uint16_t)data[2]); // Set the second parameter of pentaPianta object i
		}
	}
}

int main(void)
{
	hw.Init();
	config.baudrate = 9600 ;
 	config.periph   = UartHandler::Config::Peripheral::USART_1;
	config.stopbits      = UartHandler::Config::StopBits::BITS_1;
	config.parity        = UartHandler::Config::Parity::NONE;
	config.mode          = UartHandler::Config::Mode::RX;
	config.wordlength    = UartHandler::Config::WordLength::BITS_8;
	//config.pin_config.rx = {DSY_GPIOB, 7};  // (USART_1 RX) Daisy pin 15
	//config.pin_config.tx = {DSY_GPIOB, 6};  // (USART_1 TX) Daisy pin 14
	uart.Init(config);

	uint8_t start_bit = 'S';
	while (start_bit != 'I') // Wait for the start bit
	{
		uart.BlockingReceive(&start_bit, sizeof(start_bit), 100);
	}
	while (1)
	{
		getData(); // Call the function to get data from the UART
	}
}
