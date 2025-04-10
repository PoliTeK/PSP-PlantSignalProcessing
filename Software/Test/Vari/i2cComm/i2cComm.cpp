#include "daisy_seed.h"
#include "daisysp.h"


using namespace daisy;
using namespace daisysp;

DaisySeed hw;
/*
For the STEMMA QT cables, we follow the Qwiic convention:

Black for GND
Red for V+
Blue for SDA
Yellow for SCL
*/

int main(void)
{
	hw.Init();
	hw.StartLog (); /* true == wait for PC: will block until a terminal is connected */
	hw.DelayMs(100);
/*
	// se da problemi di compilazione la parte di config.scl e 
	// config.sda, scommentare qua sotto e assegnare questi pin
	al post di {dsy_gpio_b*/
	Pin pin_scl = {PORTB, 8};
	Pin pin_sda = {PORTB, 9};

	I2CHandle::Result res1;
	I2CHandle::Config i2c_conf;
	i2c_conf.periph = I2CHandle::Config::Peripheral::I2C_1;
	i2c_conf.speed  = I2CHandle::Config::Speed::I2C_400KHZ;
	i2c_conf.mode   = I2CHandle::Config::Mode::I2C_MASTER;
	i2c_conf.pin_config.scl  = pin_scl; // D11
	i2c_conf.pin_config.sda  = pin_sda; // D12
	I2CHandle i2c;
	res1 = i2c.Init(i2c_conf);
	
	/* FOR DEBUG*/
	
	if (res1 == I2CHandle::Result::OK){
		hw.PrintLine("I2C handler Init OK");
	}
	else {
		hw.PrintLine("I2c handler init not ok");
	}
	

	// creates object for mpr121
	daisy::Mpr121I2C::Config mpr121ObjConf;
	daisy::Mpr121I2C mpr121Obj;
	daisy::Mpr121I2C::Result res;

	// creates variable to check output of functions
	//daisy::Mpr121I2C::Result res;
	// creates config for mpr121 (constructor sets all to deafult)
	

	// init the object and checks return val
	// check not working
	res = mpr121Obj.Init(mpr121ObjConf);

	//TEST
	//mpr121Obj.SetI2cHandle();

	// DEBUG
	if (res == daisy::Mpr121I2C::Result::OK){
		hw.PrintLine("mpr121 init completed succesfully");
	}
	else {
		hw.PrintLine("mpr121 init ERROR");
		
	}
	
	// hw.PrintLine("test");
	mpr121Obj.Init(mpr121ObjConf);
	while(1) {
		// questo ritorna il registro relativo ai gate, 
		hw.PrintLine ("Gate register: %d ", mpr121Obj.FilteredData(0));
		hw.DelayMs(10);
	}
}

