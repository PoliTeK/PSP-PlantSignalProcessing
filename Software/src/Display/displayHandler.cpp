#include "displayHandler.h"


DisplayHandler::DisplayHandler (MyOledDisplay* displayPtr, bool triggerEnabled){
    // should clean circular buffer
    memset((void*) _circBuffer_ptr, 0, sizeof(_circBuffer_ptr));
    
    // sets pointer of display
    _displayPtr = displayPtr;
    _triggerEnabled = triggerEnabled;

}

void DisplayHandler::Update(){
    switch (_currentState){
        case DisplayState::STANDBY:
            drawStandbyScreen();
            break;

        case DisplayState::WAVEFORM_VIEWER:
            drawWaveForm();
            break;

        case DisplayState::POT_VALUE:
            drawPotValue();
            break;
    }
    _displayPtr->Update();
}


/* NEEDS HEAVY TUNING*/
void DisplayHandler::drawPotValue(){

    // rescaling float val -> NEEDS DEBUGGING ON BOARD TO TRIM THIS VAL
    int rescaledVal = (int) *_potValuePtr * 10.f;
    
    // chosen to be four
    char intToString_rescaledValue [4];
    sprintf(intToString_rescaledValue, "%d",rescaledVal);
    
    // write to display the string containig the resacled val
    _displayPtr->WriteString((const char*)intToString_rescaledValue,Font_16x26, true);
  
}


void DisplayHandler::SetState(DisplayState newState){

    // clears display at each new state
    _displayPtr->Fill(false);
    _currentState = newState;
}

void DisplayHandler::SetStandbyText(const char* text){
    if(text != nullptr)
    {
        _standbyText = text;
        _displayPtr->Fill(false);
    }
}


// fills buffer for waveform viewer: to be called in audio callback
void DisplayHandler::pushAudioSample(float sample){
    _circBuffer_ptr[_writeHead] = sample;

    // Implements circular buffer logic
    _writeHead = (_writeHead + 1) % BUFFER_SIZE;
}

// Trigger logic
int DisplayHandler::findTrigger(){
    float currVal = 0;
    float nextVal= 0;
    static int trigIdx = 0;

    // _writeHead is the most recent
    int oldestSampleIdx = _writeHead + 1;

    // what?
    // if (trigIdx < 0) trigIdx = BUFFER_SIZE - trigIdx;
    
    if (_triggerEnabled==true){
        

        // finds the offset from oldest sample from where to get the trigger
        for (int i = oldestSampleIdx; i < oldestSampleIdx + _windowSize; i++){
            currVal = _circBuffer_ptr[i  % BUFFER_SIZE];
            nextVal = _circBuffer_ptr[ (i+1)  % BUFFER_SIZE];
             
            // trigger condition: rising edge
            if (currVal < 0.f && nextVal >= 0.f){
                trigIdx = i  % BUFFER_SIZE;
                break; // CHANGE THIS BECAUSE BREAK IS UGLY
            }
        }
    }           
    return trigIdx;
}

/* DISPLAY FUNCTONS */

// the idea is to plot it with drawLine and decimation to avoid lagging
// STILL IN DEVELOPMENT
void DisplayHandler::drawWaveForm (){
    
    int zeroOfScreen = 32; // because is 64 pixel in heigth
    int y1, y2;
    int idx = 0;

    _displayPtr->SetCursor(0, zeroOfScreen);
    _displayPtr->Fill(false);

    idx = findTrigger();
    // plot whole buffer -> TAKES CIRCULARITY INTO ACCOUNT
    for (int i = idx; i < idx + _windowSize; i++){
        // *100 is for rescaling -> NEEDS TUNING
        // NEEDS CLAMPING LOGIC to avoid segfault
        y1 = zeroOfScreen - (_circBuffer_ptr[i % BUFFER_SIZE]* 40);
        y2 = zeroOfScreen - (_circBuffer_ptr[(i + 1) % BUFFER_SIZE] * 40);
        _displayPtr->DrawLine(i-idx, y1, (i+1)-idx, y2, true);
    }   


}



// LONG FUNCTION AHEAD -> prints test logo atm
void DisplayHandler::drawStandbyScreen(){
  _displayPtr->SetCursor(0,0);
        _displayPtr->WriteString(_standbyText, Font_11x18, true);
}