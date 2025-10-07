#include "startup.h"

#include <cstdint>

// used to check if channel has been 'touched'
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif
#define MAX_TIMEOUT_VAL_MS 300
#define MIN_OF_MAX 0 

// Macro function to avoid writing every time "ifdef then endif"
#ifdef LOGGING
    #define LOG_MSG(STR) hw.PrintLine("STARTUP:" STR)
#else
    #define LOG_MSG(STR) ((void)0)
#endif

int startup_getMaxValue (float* maxDestination,
						daisy::Mpr121<daisy::Mpr121I2CTransport>* cap_sensor,
						daisy::DaisySeed& hw,
						PlantConditioner& pc)
{
	
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
		if (((lastTouched & _BV (0)) == 0) && ((currTouched & _BV(0)) == 1 || plantIsTouched)){
			LOG_MSG ("Seems touched");
			pc.Process (cap_sensor->BaselineData(0), cap_sensor->FilteredData(0));
			hw.PrintLine("%f:", pc.getDeltaFilt());	
			//if (pc.getDeltaFilt() > 5){
				plantIsTouched = true;
				LOG_MSG("Sensor at channel 0 has been touched");
			//}
		}
		lastTouched = currTouched;
	}

	/* Cycle for a prefixed time to capture the maximum value */
	/// @todo fix argument of for loop, should work but is ugly	
	/// @todo check if this method is valid, maybe better to take mean value?
	int num_of_sums = 0; 
	LOG_MSG("Starting measurement of maximum, wait for led to blink 3 times");
	for (int time = 0; time < MAX_TIMEOUT_VAL_MS; time++){
		pc.Process (cap_sensor->BaselineData(0), cap_sensor->FilteredData(0));// hw.adc.GetFloat(0);
		if(pc.getDeltaFilt() > 1){
			currentValue += pc.getDeltaFilt();
			num_of_sums++;
		}
		hw.PrintLine("%f:", pc.getDeltaFilt());
		hw.DelayMs(10);
	}
	LOG_MSG ("Ended measurements, writing value to max");
	
	// calculate mean of values
	maxValue = currentValue /num_of_sums;

	// write result to given destination only if makes sense
	maxValue >= MIN_OF_MAX ? *maxDestination = maxValue: error = -1; 

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
