/**
 * @brief Test main function for dynamic thresholds
 * 
 * This is the main entry point for the display test application.
 * It serves as a test harness to verify dynamic threshold functionality.
 * 
 * @todo use new function SetThreshold to change only one channel
 * 
 * @return int Exit status code (0 for success, non-zero for failure)
 */
 

#include "../../libs/libDaisy/src/daisy_seed.h"
#include "../../libs/DaisySP/Source/daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;


// unused in this test
static void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{

    for (size_t i = 0; i < size; i += 2)
    {

        out[i] = out[i + 1] = 0.f;             
    }
}

int main (){

    hw.Init();    
    
    // mpr121 creation
    Mpr121I2C::Config mpr121ObjConf;
    Mpr121I2C         cap;
    
    // init log to pc, program waits for serial monitor connection
    hw.StartLog(true);
    hw.PrintLine ("Entering program...");

    uint8_t touchThreshold = 0;
    uint8_t releaseThreshold = 0;
    uint8_t newTh = 0;
    int iteration = 0;
    
    // mpr121 initialization
    mpr121ObjConf.transport_config.speed = I2CHandle::Config::Speed::I2C_400KHZ;
    if (cap.Init(mpr121ObjConf) != Mpr121I2C::Result::OK)
    {
        while (1) {
            hw.SetLed(true); hw.DelayMs(100);
            hw.SetLed(false); hw.DelayMs(100);
        }
    }
    hw.SetLed(true);

    // sets starting thresholds
    cap.SetThresholds (10, 20);
    
    while (1){

        // read thresholds
        touchThreshold = cap.ReadRegister8 (Mpr121I2C::RegMap::MPR121_TOUCHTH_0);
        releaseThreshold = cap.ReadRegister8 (Mpr121I2C::RegMap::MPR121_RELEASETH_0);
        
        // print thresholds
        hw.PrintLine ("%d iter: touch is %d and release is %d", iteration, touchThreshold, releaseThreshold);

        // change thresholds at 10th iteration 
        if (iteration == 10) {
            newTh = 30;
            cap.SetThresholds(newTh, newTh);
        }
        // update iter
        iteration++;

        // delay
        hw.DelayMs(500);
    }
    
    return 0;
 }