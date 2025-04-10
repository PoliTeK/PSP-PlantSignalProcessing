#include <Wire.h>
#include "Adafruit_MPR121.h"
#include "pentaPianta.h"

#define CHANNEL 1

#ifndef _BV  //used to mask registers' bits related to the channel
#define _BV(bit) (1 << (bit)) 
#endif
Adafruit_MPR121 cap = Adafruit_MPR121();

struct Pianta{
  uint8_t gate;
  uint16_t value;
};
Pianta piante[7];
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
    Serial.write('S');
    while (1);
  }
  for(int i=0; i<3; i++)
    Serial.write('B');  
}

void loop() {
  for(int i=0; i<7; i++)
  {
    piante[i].gate=cap.touched() & _BV(i);
    piante[i].value=cap.filteredData(i);
    if(piante[i].value == 'B') piante[i].value++;
  }
  if(Serial.availableForWrite())
  {
    Serial.write('B');
     for(int i=0; i<7; i++)
    {
      Serial.write(piante[i].gate);
      Serial.write((uint8_t *)&piante[i].value,2);
    }
  }
  delay(1);
}