## DOCUMENTAZIONE CODICE

### Documentazione Tecnica (Prima Iterazione)
- [AudioEngine](docs/AudioEngine.md): struttura classe, API pubblica, pipeline DSP e template per le prossime classi.
- [AudioEngineConfig](docs/AudioEngineConfig.md): parametri principali dei preset PAD/PLUCK/LEAD e riverbero.

### Documentazione Tecnica (Estensione)
- [PlantConditioner](docs/PlantConditioner.md): mapping sensore capacitivo -> frequenza/gate con scale musicali e isteresi.
- [Main Runtime Flow](docs/Main.md): orchestrazione di startup, integrazione tra moduli, control-rate loop e callback audio-rate.

### Documentazione Tecnica (Display/UI)
- [DisplayHandler](docs/DisplayHandler.md): gestione OLED, waveform viewer e schermate menu.
- [MenuManager](docs/MenuManager.md): macchina a stati UI e parametri controllati da encoder.








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
