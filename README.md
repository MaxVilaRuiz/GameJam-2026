# MegaMask - GameJam 2026

**MegaMask** is a 2D action survival game developed during **GameJam 2026**.

You play as a monk trapped in a zombie apocalypse caused by mysterious parasitic masks. By using these masks, the monk can unleash powerful elemental magic and fight against endless waves of enemies.

The game follows a **round-based survival system**: zombies continuously spawn and become stronger with each wave. Between rounds, players can visit a merchant to switch masks and unlock different magical abilities.

Players can use four elemental powers:

- Fire
- Water  
- Rock  
- Wind  

Each element offers **unique attacks and abilities**, allowing players to adapt their strategy as enemies become more powerful.

Survive as long as possible, experiment with different masks, and defeat increasingly dangerous hordes of zombies.

<br>


## Technology Notes

MegaMask was built using a **custom C++ game engine** developed during the hackathon.

Main technologies used:

- **C++**
- **SDL2**
- **SDL2_image**
- **SDL2_mixer**
- **CMake**

The game uses SDL for rendering, asset loading, audio management, and input handling.

<br>


## Installation & Build Guide

### Clone the repository

```bash
git clone https://github.com/YOUR_USERNAME/MegaMask.git
cd MegaMask
```

### Requirements
Make sure the following libraries are installed on your system:
- SDL2
- SDL2_image
- SDL2_mixer
- CMake
- C++ compiler (GCC / Clang / MSVC)

### Build the project
```bash
mkdir build
cd build
cmake ..
make
```

### Run the game
```bash
./MegaMask
```

<br>


## Platforms
MegaMask can be built and run on:
- **Linux / UNIX**
- **macOS**
- **Windows**

(As long as SDL2 dependencies are installed)

<br>


## Team & Credits
- [Max Vilà Ruiz](https://github.com/MaxVilaRuiz)
- [Pau Martínez Franch](https://github.com/taopaipau)
- [Aarón Quintanilla](https://github.com/aaronqintanilla)
- [Max Gimeno Giro](https://github.com/Max-Gimeno-G)

<br>


## License
This project is licensed under the [Apache 2.0](https://choosealicense.com/licenses/apache-2.0/) © License.
