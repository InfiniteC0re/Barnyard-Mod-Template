# Barnyard Mod Template
[OpenBarnyard](https://github.com/InfiniteC0re/OpenBarnyard) | [Barnyard Launcher](https://github.com/InfiniteC0re/BarnyardLauncher) | [YouTube](https://www.youtube.com/@infc0re) | [Discord](https://discord.gg/3bMq8hWvFX)

A template you can use to set up blank mod projects for Barnyard the video game.
Requires BYModLoader in order to start the mod.

## Notes

> [!IMPORTANT]
> **SUPPORT THE DEVELOPERS OF THE TOSHI ENGINE**
> 
> We do not own the Toshi Engine in any way, shape or form, and this project would not have been possible had they not developed TOSHI in the first place. The Toshi Engine is currently owned by [THQ Nordic](https://www.thqnordic.com); we highly urge you to follow & support their projects if you like this project!

> [!WARNING]
> **DO NOT USE THIS DECOMPILATION PROJECT AS A MEANS TO PIRATE BARNYARD OR ANY OTHER TOSHI GAMES!**
> 
> We do not condone using this project as a means for piracy in any form. This project was made with love and care for the source material and was created for purely educational purposes. 

## Usage Guide

1. Instantiate a new repository from this template
2. Create OpenBarnyard folder (1) somewhere on your disk
3. Create Game folder in the previously created directory (1)
4. Copy original game assets and the BYardModLoader.exe (get it from the Winter Mod or somewhere else) file into the Game folder
5. Clone your repository, placing it's folder in the OpenBarnyard directory
6. At this moment the structure should look like this:

    ```
    .
    └── OpenBarnyard/
        ├── Game/
        │   ├── Data
        │   ├── Saves
        │   ├── BYardModLoader.exe
        │   └── ...
        └── YourRepositoryName
    ```
7. CD into your repository folder
8. Execute `GenerateProjects-WIN-VS2022.bat` in order to generate file projects
9. Open the generated solution file