#include "../libs/PoliTeKDSP/libs/libDaisy/src/daisy_seed.h"
#include "../libs/PoliTeKDSP/libs/DaisySP/Source/daisysp.h"
#include "../libs/PoliTeKDSP/libs/libDaisy/src/hid/encoder.h"
#include "../libs/PoliTeKDSP/libs/libDaisy/src/util/scopedirqblocker.h"
#include "Display/displayHandler.h"
#include "PSP/PlantConditioner.h"
#include "PSP/AudioEngine.h"

using namespace daisy;

// Istanze hardware e software
DaisySeed hw;
PlantConditioner pc;
AudioEngine synth;
MyOledDisplay display;
DisplayHandler display_handle(&display);
TimerHandle display_timer;
TimerHandle encoder_timer;
Encoder menu_encoder;

// Variabile globale condivisa tra il main loop e la callback audio
ControlsStruct audio_controls = {440.0f, false};
volatile bool display_refresh_due = false;
volatile int encoder_increment_isr = 0;
volatile int encoder_click_count_isr = 0;
volatile bool encoder_pressed_isr = false;
constexpr auto DISPLAY_REFRESH_HZ = 30u;
constexpr auto ENCODER_POLL_HZ = 2000u;

const char* kMenuLabels[] = {
    "PoliTeK PSP",
    "PSP",
    "Signal Processing",
    "PoliTeK 23",
};

constexpr int kMenuLabelCount = sizeof(kMenuLabels) / sizeof(kMenuLabels[0]);
int current_menu_label = 0;

void DisplayRefreshTimerCallback(void* data) {
    (void)data;
    display_refresh_due = true;
}

void EncoderPollTimerCallback(void* data) {
    (void)data;
    menu_encoder.Debounce();

    const int inc = menu_encoder.Increment();
    if(inc != 0)
    {
        int sum = encoder_increment_isr + inc;
        if(sum > 1024)
            sum = 1024;
        else if(sum < -1024)
            sum = -1024;
        encoder_increment_isr = sum;
    }

    if(menu_encoder.RisingEdge())
        encoder_click_count_isr++;

    encoder_pressed_isr = menu_encoder.Pressed();
}

// Callback Audio: eseguita dal processore ogni volta che serve riempire il buffer audio
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    // 1. Aggiorna il sintetizzatore con gli ultimi dati disponibili letti dalla pianta
    synth.Update(audio_controls);

    // 2. Genera l'audio per la dimensione del buffer corrente
    for (size_t i = 0; i < size; i++) {
        // Calcola il singolo campione
        float sample = synth.Process();
        
        // Invia lo stesso campione su entrambi i canali di uscita (Dual Mono)
        out[0][i] = sample; // Left
        out[1][i] = sample; // Right
    }
}

int main() {
    // Inizializzazione Hardware
    hw.Init();

    // Inizializzazione Display OLED
    MyOledDisplay::Config display_cfg;
    display.Init(display_cfg);
    display_handle.SetState(DisplayState::STANDBY);
    display_handle.SetStandbyText(kMenuLabels[current_menu_label]);
    display_handle.Update();

    // Timer hardware per refresh periodico del display
    TimerHandle::Config display_timer_cfg;
    display_timer_cfg.periph     = TimerHandle::Config::Peripheral::TIM_5;
    display_timer_cfg.enable_irq = true;
    display_timer.Init(display_timer_cfg);
    display_timer.SetCallback(DisplayRefreshTimerCallback);

    auto display_period_ticks = display_timer.GetFreq() / DISPLAY_REFRESH_HZ;
    if(display_period_ticks == 0)
        display_period_ticks = 1;
    display_timer.SetPeriod(display_period_ticks);
    display_timer.Start();

    // Inizializzazione Encoder con pulsante
    menu_encoder.Init(seed::D14, seed::D15, seed::D10, ENCODER_POLL_HZ);

    // Timer dedicato per polling/debounce encoder (separato dal timer display)
    TimerHandle::Config encoder_timer_cfg;
    encoder_timer_cfg.periph     = TimerHandle::Config::Peripheral::TIM_4;
    encoder_timer_cfg.enable_irq = true;
    encoder_timer.Init(encoder_timer_cfg);
    encoder_timer.SetCallback(EncoderPollTimerCallback);

    auto encoder_period_ticks = encoder_timer.GetFreq() / ENCODER_POLL_HZ;
    if(encoder_period_ticks == 0)
        encoder_period_ticks = 1;
    encoder_timer.SetPeriod(encoder_period_ticks);
    encoder_timer.Start();
    
    // Configurazione blocco audio per bassa latenza (opzionale ma consigliato per i synth)
    hw.SetAudioBlockSize(16);

    // Inizializzazione PlantConditioner
    pc.Init(IIR::Butterworth, &hw);
    pc.setDelta(50.0f);
    pc.setCurve(1.0f);
    pc.setScale(PlantConditioner::G, PlantConditioner::Major);
    pc.setOctave(4);

    // Inizializzazione AudioEngine
    synth.Init(hw.AudioSampleRate());
    synth.SetPreset(PRESET_LEAD);

    // Avvia la callback audio in background
    hw.StartAudio(AudioCallback);

    while(1) {

        PlantConditioner::PlantState plant_data = pc.Process();

        audio_controls.freq = plant_data._freq;
        audio_controls.gate = plant_data._gate;

        int encoder_increment = 0;
        int encoder_click_count = 0;
        {
            ScopedIrqBlocker irq_blocker;
            encoder_increment = encoder_increment_isr;
            encoder_increment_isr = 0;
            encoder_click_count = encoder_click_count_isr;
            encoder_click_count_isr = 0;
            (void)encoder_pressed_isr;
        }

        if(encoder_increment != 0)
        {
            current_menu_label += encoder_increment;
            while(current_menu_label < 0)
                current_menu_label += kMenuLabelCount;
            while(current_menu_label >= kMenuLabelCount)
                current_menu_label -= kMenuLabelCount;

            display_handle.SetStandbyText(kMenuLabels[current_menu_label]);
        }

        if(encoder_click_count > 0)
        {
            current_menu_label = 0;
            display_handle.SetStandbyText(kMenuLabels[current_menu_label]);
        }

        if(display_refresh_due) {
            display_refresh_due = false;
            display_handle.Update();
        }

        // 3. Pausa di 1 ms per stabilizzare il bus I2C e non saturare la CPU
        System::Delay(1);
    }
}