#include "../../libs/libDaisy/src/daisy_seed.h"

using namespace daisy;

DaisySeed hw;
Encoder   enc;

// Variabili di test per simulare i tuoi parametri
float test_delta = 50.0f;
float test_curve = 1.1f;
int   menu_mode  = 0; // 0: Delta, 1: Curve

int main()
{
    // 1. INIZIALIZZAZIONE HARDWARE
    hw.Init();
    
    // Avvia la comunicazione seriale e attende che l'utente apra il monitor
    // Se passi 'true', la Daisy aspetterà finché non connetti il terminale
    hw.StartLog(true); 
    hw.PrintLine("=== TEST ENCODER AVVIATO ===");
    hw.PrintLine("Usa D13 (A), D14 (B) e D10 (Click)");

    // 2. INIZIALIZZAZIONE ENCODER
    // Configurazione: Pin A, Pin B, Pin Click
    enc.Init(hw.GetPin(13), hw.GetPin(14), hw.GetPin(10));

    while(1)
    {
        // 3. AGGIORNAMENTO STATO (Debounce)
        // Questa funzione deve essere chiamata costantemente
        enc.Debounce();

        // 4. TEST CLICK (RisingEdge)
        if(enc.RisingEdge())
        {
            menu_mode = (menu_mode + 1) % 2;
            hw.PrintLine(">>> CAMBIO MENU: %s", (menu_mode == 0 ? "DELTA" : "CURVE"));
            
            // Feedback visivo sul LED della Daisy
            hw.SetLed(menu_mode == 0); 
        }

        // 5. TEST ROTAZIONE (Increment)
        int32_t inc = enc.Increment();
        if(inc != 0)
        {
            if(menu_mode == 0)
            {
                test_delta += (inc * 5.0f);
                hw.PrintLine("Modifica DELTA: %.2f", test_delta);
            }
            else
            {
                test_curve += (inc * 0.1f);
                hw.PrintLine("Modifica CURVE: %.2f", test_curve);
            }
        }

        // Piccolo delay per non saturare la CPU (circa 1000Hz)
        System::Delay(1);
    }
}