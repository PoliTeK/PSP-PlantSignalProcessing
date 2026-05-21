/** Generation of a simple Audio signal */
#include "daisy_seed.h"
#include "flash_utils.h"

/** This prevents us from having to type "daisy::" in front of a lot of things. */
using namespace daisy;

DaisySeed hw;


int main(void)
{
    /** Initialize our hardware */
    hw.Init();

    /** Start Log */
    hw.StartLog(true);
    System::Delay(2000); // Pausa per aprire il monitor seriale
    hw.PrintLine("Inizio Test Flash QSPI...");

	
	uint8_t value;
	FlashUtils::read(0, value);
	hw.PrintLine("Valore letto: %d", value);

	value++;
	hw.PrintLine("Scrivo valore: %d", value);
	FlashUtils::write(0, value, hw);

    /** Infinite Loop */
    while(1)
    {
        // Blink LED per indicare fine test
        hw.SetLed(true);
        System::Delay(500);
        hw.SetLed(false);
        System::Delay(500);
    }
}
