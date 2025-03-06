#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "pentaPianta.h"

#define CHANNEL 1

#ifndef _BV  //used to mask registers' bits related to the channel
#define _BV(bit) (1 << (bit)) 
#endif
Adafruit_MPR121 cap = Adafruit_MPR121();

// FOR ANY PLANT CHOOSE THE TRESHOLDS!!
int soglie[7]={5, 10, 20, 40, 60, 80, 100 }; // the first 2 are touch and release
pentaPianta p=pentaPianta(soglie,440); // creating the penta pianta object

//SETUP IS THE OF THE MPR121test file
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);

  while (!Serial) { // needed to keep micro from starting too fast!
    delay(10);
  }
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");  
}

void loop() {
  uint16_t frequency,value; 
  while(!( cap.touched() & _BV(CHANNEL))); // If the channel isn't touched it waits
   delay(10);//wait the value to stabilize on a value
    value=cap.baselineData(CHANNEL)-cap.filteredData(CHANNEL);//The baseline data 
  //is the enviroment  noise and is set again every time we call cap.begin()
    frequency=p.discFreq(value); //discFreq return the frequency inside the scale

    Serial.print("The value of the freq is ");
    Serial.println(frequency);

    delay(1000); //before going in glide mode we wait some time (to be calibrate)
    
    while((cap.touched()& _BV(CHANNEL))) // if the sensor is still touched we proced
    {
      value=cap.baselineData(CHANNEL)-cap.filteredData(CHANNEL);
      frequency = p.analogFreq(value);
  
      Serial.print("The value of the freq ");
      Serial.println(frequency);
    }
}
