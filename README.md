# PSP-PlantSignalProcessing

## Configurazione
```bash
git clone https://github.com/PoliTeK/PSP-PlantSignalProcessing.git
cd PSP-PlantSignalProcessing
git submodule update --init --recursive
```

## Struttura del Progetto
```
PSP/
├── Hardware/
│   ├── Schema_a_blocchi/
│   ├── Biopotential/
│   └── Componenti/
│
└── Software/
    ├── Test/                 # Directory temporanea per i test
    │   ├── Arduino/         # File di test Arduino
    │   ├── Vari/           # Test vari
    │   └── Template/       # Cartella template per nuove classi
    │
    ├── src/                 # File principali progetto
    ├── Utility/            # Classi di utilità (sensori, smoothing)
    ├── Effects/            # Effetti audio
    ├── Synth/             # Motore audio
    └── libs/              # Librerie esterne
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

## Linee Guida per lo Sviluppo

### Aggiunta di Nuove Classi
1. Iniziare copiando la cartella template:
   ```bash
   cp -r Software/Test/Template Software/[cartella_destinazione]/[nome_nuova_classe]
   ```
2. Rinominare i file nella nuova cartella:
   - `template.cpp` → `[nome_nuova_classe].cpp`
   - `template.h` → `[nome_nuova_classe].h`

3. Aggiornare il nome della classe in entrambi i file

4. Utilizzare main.cpp per testare la classe sulla scheda DaisySeed creando un esempio di utilizzo

5. Per compilare l'esempio, assicurarsi di aggiornare correttamente il Makefile e seguire le stesse istruzioni di build del progetto principale

6. CREARE UN FILE README PER SPIEGARE COME UTILIZZARE LA CLASSE E I SUOI METODI PUBBLICI

7. Se funziona correttamente, aggiungere l'implementazione nelle seguenti posizioni:
   - Gli effetti vanno nella cartella `Software/Effects`
   - I componenti del sintetizzatore vanno in `Software/Synth`
   - Le utilità vanno in `Software/Utility`

8. Includere i nuovi file nel progetto principale se necessario

### Stile del Codice
- **camelCase** per i nomi dei metodi
  - La prima lettera è minuscola
  - Ogni parola successiva inizia con la maiuscola
  - Esempi: 
    - `setFrequency()`
    - `setVolume()`
    - `readSensorValue()`

- **PascalCase** per i nomi delle classi
  - Ogni parola, inclusa la prima, inizia con la maiuscola
  - Esempi:
    - `class CapacitiveSensor`
    - `class ReverbEffect`

- Prefissare i membri privati con underscore (es. `_variabilePrivata`, `_metodoPrivato`)
- Includere documentazione per i metodi pubblici

### Linee Guida per l'Uso di Git
- Prima di iniziare a lavorare, aggiornare sempre con gli ultimi cambiamenti:
  ```bash
  git pull origin main
  ```

- Creare un nuovo branch per la feature/fix:
  ```bash
  git checkout -b feature/nome-feature
  # oppure
  git checkout -b fix/nome-fix
  ```

- Committare frequentemente con messaggi significativi:
  ```bash
  git add .
  git commit -m "Descrizione delle modifiche"
  ```

- Mantenere il branch aggiornato con il main:
  ```bash
  git checkout main
  git pull origin main
  git checkout nome-branch
  git merge main
  ```

- Quando si è pronti a condividere il lavoro:
  ```bash
  git push origin nome-branch
  ```
  Quindi creare una Pull Request su GitHub

- Migliori Pratiche:
  - Fare pull all'inizio di ogni sessione di lavoro
  - Committare frequentemente con messaggi chiari
  - Testare le modifiche prima del push
  - Rivedere il codice prima di creare una Pull Request
  - Risolvere i conflitti localmente prima del push
  - Mantenere i commit focalizzati e fotonici
