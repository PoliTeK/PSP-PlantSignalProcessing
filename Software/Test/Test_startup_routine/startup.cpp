#include "startup.h"
#include <cstdint>

// used to check if channel has been 'touched'
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif
#define MAX_TIMEOUT_VAL_MS 2000

// Macro function to avoid writing every time "ifdef then endif"
#ifdef LOGGING
    #define LOG_MSG(STR) hw.PrintLine("STARTUP:" STR)
#else
    #define LOG_MSG(STR) ((void)0)
#endif

int startup_getMaxValue (float* maxDestination,
						 daisy::Mpr121<daisy::Mpr121I2CTransport>* cap_sensor,
						 daisy::DaisySeed& hw){
	
	LOG_MSG ("Entered startup routine");
	int error = 0;
	uint16_t lastTouched = 0; 
	uint16_t currTouched = 0;
	float currentValue = 0;
	float maxValue = 0;
	// uint32_t timer_start = daisy::System::GetUs();
	
	/* Keeps cycling until the sensor is touched */
	/// @todo add a timeout to avoid infinite loop 
	LOG_MSG ("Waiting for sensor to be touched at channel 0");
	LOG_MSG ("Touch it in the maximum to get correct reading");
	bool plantIsTouched = false; 
	while (plantIsTouched == false){

		// get value of touched register (all channels)
		currTouched = cap_sensor->Touched();

		// check if bit of channel has gone from 0 to 1
		if (((lastTouched & _BV (0)) == 0) && ((currTouched & _BV(0)) == 1)){
			plantIsTouched = true;
			LOG_MSG("Sensor at channel 0 has been touched");
		}
		lastTouched = currTouched;
	}

	/* Cycle for a prefixed time to capture the maximum value */
	/// @todo fix argument of for loop, should work but is ugly	
	/// @todo check if this method is valid, maybe better to take mean value? 
	LOG_MSG("Starting measurement of maximum, wait for led to blink 3 times");
	for (int time = 0; time < MAX_TIMEOUT_VAL_MS; time++){
		currentValue = hw.adc.GetFloat(0);
		if(currentValue >= maxValue) {maxValue=currentValue;}
		// for now it takes measurement every millisecond (not so great approach)
		hw.DelayMs(1);
	}
	LOG_MSG ("Ended measurements, writing value to max");
	
	// write result to given destination only if makes sense
	maxValue > 0? *maxDestination = maxValue: error = -1; 

	// blink 3 times to show correct closing of procedure
	LOG_MSG ("END OF PROCEDURE");
	for (int i = 0; i < 3; i++){
		hw.SetLed(true);
		hw.DelayMs(200);
      	hw.SetLed(false);
      	hw.DelayMs(200);    
	}
	return error;
}
