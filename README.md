# KlinKlang-Engine
Pokémon White 2 Engine to enhance CTRMap features. The KlinKlang engine can attach to your CTRMap project and help editing common NARC files and even build simple PMC patches.

## How to use
To create a project you will first need a CTRMap project, so create that first.
 1. Open KlinKlang Engine and click the ``Create Project`` button.
 2. Select the CTRMap project file to attach to.
Once you have a project you can open it clicking the ``Open Project`` button.
Whenever you save the KlinKlang project, the changes you made will be transfered to your CTRMap project.

## Features
- Fully functional Ctrl+Z, nothing crazy but it's nice.
- Pokémon data editor: Modify any aspect of the data of a Pokémon, from the type to the Pokémon that gets generated when it makes an egg.
- Pokémon data search engine: Although still not complete, even if you don't want to modify any data, this engine allows you to sort the Pokémon data in many useful  ways (sometimes you want to know which Pokémon learn Iron Head at level 37).
- PMC patch builder: Provided you download the necesary dependences, you can build a Code Injection patch with the click of a button. Very useful for people unfamiliar with PMC that only want to use pre-made patches.

## Missing Features
 - Move data editor
 - Trainer data editor
 - Encounter data editor
 - Pokémon graphics visualizer (maybe even a palette editor)

## How to build PMC patches
Before installing any patches you will need to [install PMC](https://ds-pokemon-hacking.github.io/docs/generation-v/guides/bw_b2w2-code_injection/#setting-up-the-environment) in your CTRMap project.

The patch needs to be in the ``KlinKlangEngine/patches`` folder with the following structure:
* PatchName
  * source
    * "source files" (cpp files must be in this folder)
    * settings.h (if you have togglabe defined values put them in this file so that the engine can read them)
  * ESDB file

If the patch is correctly set up you should see it under the ``Patcher`` menu bar section.
Click the patches buttons open a menu to enable them and modify the settings.

To build the patch first go to ``Patcher``->``Build Settings`` and fill the required dependency paths:
 1. Compiler Path: path to the arm-none-eabi forlder, leave this empty if you have it as a System path.
 2. Java Path: path to the java forlder, leave this empty if you have it as a System path.
 3. CTRMap Path: path where ``CTRMap.jar`` is located in your computer.
 4. SWAN Path: path where the SWAN header files are located.
 5. NitroKernel Path: path where the NitroKernel include files are located.
 6. ExtLib Path: path where the ExtLib header files are located.
 7. LibRPM Path: path where the LibRPM include files are located.

When everything is correctly set up, click ``Patcher``->``Build`` to build and install the patch.
If everything worked correctly you should see the DLL file of the patch in the CTRMap project ``patches`` folder.
