# Overview
In this folder there is a simple example showing the capabilities of doxygen inside a cpp project.

# Installing Doxygen 
To install Doxygen on Windows, the following link must be taken as a reference: https://www.doxygen.nl/manual/install.html, specifically the section "installing the binaries on Windows".
Next, to have Doxygen integrated with VScode it will be best to install the 'Doxygen' extension by Baptiste BENOIST, from the extension manager.
To check if installation went fine, type in the vscode terminal the keywork 'doxygen', if doxygen complain about wrong usage then it is intalled but if the terminal complains that the keyword doesn't exist then something went wrong.

# Usage
In the directory there are 2 very simple classes in cpp made to showcase some simple doxygen commands. The idea is that the commands shown in this example will usually be enough for the most part of a project.
The idea is that writing the usual comments for the code (albeit a bit more detailed) will result automatically in writing the documentation!

## Doxygen comment
Every doxygen comment starts either with /// or with /** (for blocks) followed by an @ and an intuitive keyword. Depending on the keyword some text can be entered afterward.
For example, the command /// @brief is used to give a short description about a method, a class, a file or anything else (refer to the examples in this directory for better understanding).

There are a large number of commands in doxygen to do some fancy stuff, which are not useful 99% of the time, so it is better to stick with few but useful commands, like the ones in the example.

# Creating the documentation
After commenting the code, a couple steps are required to generate the documentation:

## first step: Creating the Doxyfile
The Doxyfile is created simply by calling the command "doxygen -g" from the terminal. The Doxyfile is a configuration file where one can write the name of the project, in which format the documentation must be, which file to ignore while creating it etc.
For the examples, only the flag PROJECT_NAME was changed and the CREATE_HTML was turned off. Check that doxygen has actually created it.

## second step: Creating the Latex
After creating and setting up the doxyfile, to create the documentation simply call the command "doxygen" from the cli. 
In the example, the 'latex' output was chosen, so inside the directory 'latex' there are all the files that, when compiled for example with Overleaf, produce the pdf that is found  here under the name 'test_doc.pdf' 
