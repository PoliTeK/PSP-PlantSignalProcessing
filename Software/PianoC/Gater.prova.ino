#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "pentaPianta.h"

#define CHANNEL 1

#ifndef _BV  //used to mask registers' bits related to the channel
#define _BV(bit) (1 << (bit)) 
#endif
Adafruit_MPR121 cap = Adafruit_MPR121();


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
int gate[7];
void loop() {
  uint16_t frequency,value; 
  for (int i=0; i<7; i++)
  {
    if(cap.touched() & _BV(i)){
      gate[i]=1;
    }
    else {
      gate[i]=0;
    }
  }
   for (int i=0; i<7; i++)
  {
    if (gate[i] == 1){
      Serial.print("Hai premuto il gate  ");
      Serial.println(i);
    }
    
  }
    delay(100); //before going in glide mode we wait some time (to be calibrate)
    
 
}
