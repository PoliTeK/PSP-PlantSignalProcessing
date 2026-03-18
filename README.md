# PSP-PlantSignalProcessing

## Configurazione
```bash
git clone https://github.com/PoliTeK/PSP-PlantSignalProcessing.git
cd PSP-PlantSignalProcessing
git submodule update --init --recursive
```

## Struttura del Progetto
```
PSP-PlantSignalProcessing/
├── Hardware/
│   ├── BOM/
│   │   └── README.md
│   └── PCB/
│       └── README.md
│
└── Software/
    ├── libs/
    │   └── PoliTeKDSP/
    │       ├── Effects/
    │       ├── Modulators/
    │       ├── Others/
    │       ├── Template/
    │       ├── Utilities/
    │       └── libs/
    │
    └── src/                    # File principali progetto
        ├── Components/
        ├── Display/
        ├── PSP/
        ├── build/              # File compilati (.o, .lst, .d)
        ├── main.cpp
        ├── Makefile
        └── README.md
```

## Istruzioni di Build

### Prerequisiti
- VS Code installato 
- Seguire la [Guida di Configurazione Iniziale](https://github.com/electro-smith/DaisyWiki/wiki#1-upload-an-example-program) per VS Code e l'ambiente Daisy
- Seguire la guida al link https://registry.platformio.org/libraries/adafruit/Adafruit%20MPR121/installation per l'installazione della libreria per il sensore capacitivo utilizzando l'ambiente PlatformIO su VSCode
- Assicurarsi che i driver USB DFU siano installati per la scheda Daisy (**Attenzione: questo passaggio è spesso fonte di bestemmie!**)

### Compilazione del Progetto
1. Aprire il progetto in VSCode e aprire il terminale
2. Navigare nella cartella Software/src: `cd Software/src`
3. Per la prima compilazione, è necessario buildare le librerie usando il comando `make all`
4. Se tutto è compilato con successo, collegare il Daisy Seed al PC via USB, metterla in modalità DFU usando i pulsanti boot e reset, quindi eseguire `make program-dfu` per caricare il programma

### Compilazione delle Librerie
A volte il comando `make all` non riesce a compilare le librerie. In questo caso, compilarle manualmente:
1. Navigare nella directory libs:
   ```bash
   cd Software/libs
   cd DaisySP
   make clean
   make
   cd ..
   cd libDaisy
   make clean
   make
   ```
2. Se non si verificano errori, procedere con la compilazione del progetto

