# PSP-PlantSignalProcessing

## Setup
```bash
git clone https://github.com/PoliTeK/PSP-PlantSignalProcessing.git
cd PSP-PlantSignalProcessing
git submodule update --init --recursive
```

## Project Structure
```
PSP/
├── Hardware/
│   ├── Schema_a_blocchi/
│   ├── Biopotential/
│   └── Componenti/
│
└── Software/
    ├── Test/                 # Temporary testing directory
    │   ├── Arduino/         # Arduino test files
    │   ├── Vari/           # Miscellaneous tests
    │   └── Template/       # Template folder for new classes
    │
    ├── src/                 # Main Daisy source files
    ├── Utility/            # Utility classes (sensors, smoothing)
    ├── Effects/            # Audio effects
    ├── Synth/             # Audio engine
    └── libs/              # External libraries
```

## Build Instructions

### Prerequisites
- VS Code installed 
- Follow [Initial Setup Guide](https://github.com/electro-smith/DaisyWiki/wiki#1-upload-an-example-program) for VS Code and Daisy environment
- Make sure DFU USB drivers are installed for your Daisy board (**Warning: this step is known to cause severe headaches and testing one's patience!**)

### Building the Project
1. Open the project in VSCode and open the terminal
2. Navigate to Software/src folder: `cd Software/src`
3. For first-time compilation, you need to build the libraries using the command `make all`
4. If everything compiled successfully, connect the Daisy Seed to your PC via USB, put it in DFU mode using the boot and reset buttons, then run `make program-dfu` to upload the program

### Building Libraries
Sometimes the `make all` command fails to compile libraries. In this case, build them manually:
1. Navigate to the libs directory:
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
2. If no errors occur, proceed with project compilation

## Development Guidelines

### Adding New Classes
1. Start by copying the template folder:
   ```bash
   cp -r Software/Test/Template Software/[destination_folder]/[new_class_name]
   ```
2. Rename the files inside the new folder:
   - `template.cpp` → `[new_class_name].cpp`
   - `template.h` → `[new_class_name].h`

3. Update the class name in both files to match your new class name

4. Use main.cpp to test the class on the DaisySeed board by creating a usage example

5. To compile the example, make sure to properly update the Makefile and then follow the same build instructions as the main project, but in your working directory

6. CREATE A README FILE TO EXPLAIN HOW TO USE THE CLASS AND ITS PUBLIC METHODS

7. If working correctly, add your implementation in the following locations:
   - Effects go in the `Software/Effects` folder
   - Synth components go in the `Software/Synth` folder
   - Utilities go in the `Software/Utility` folder

8. Include your new files in the main project if needed

### Code Style
- Use camelCase for method names
- Use PascalCase for class names
- Prefix private members with underscore (e.g., _privateVariable, _privateMethod)
- Include documentation for public methods

### Git Workflow Guidelines
- Before starting to work, always pull the latest changes:
  ```bash
  git pull origin main
  ```

- Create a new branch for your feature/fix:
  ```bash
  git checkout -b feature/your-feature-name
  # or
  git checkout -b fix/your-fix-name
  ```

- Commit your changes frequently with meaningful messages:
  ```bash
  git add .
  git commit -m "Description of your changes"
  ```

- Keep your branch updated with main:
  ```bash
  git checkout main
  git pull origin main
  git checkout your-branch-name
  git merge main
  ```

- When ready to share your work:
  ```bash
  git push origin your-branch-name
  ```
  Then create a Pull Request on GitHub

- Best Practices:
  - Pull changes at the start of each working session
  - Commit frequently with clear messages
  - Test your changes before pushing
  - Review your code before creating a Pull Request
  - Resolve conflicts locally before pushing
  - Keep commits focused and atomic
