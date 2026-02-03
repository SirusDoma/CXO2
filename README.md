# CXO2

**Client Version: Pre-NX (v3.10)** (3.15 or 3.38 may work, but largely untested)

A faithful, customizable, and cross-platform implementation of O2Jam client written in C++ on top of [Genode](https://github.com/SirusDoma/Genode),
an in-house game engine based on [SFML](https://github.com/SFML/SFML/).

Unlike other remake or clone projects, the project is made to be a faithful, literal "clone"/"carbon copy".
This includes everything we know about O2Jam, including but not limited to: interface, avatar, asset formats, network protocol and online system.

It is not aimed to be a 1:1, which means some minor design/details may deviate slightly from the original O2Jam,
But the project will always remain faithful to the original O2Jam.

The end goal is to remake every official major clients published by e-Games and Nowcom.

## Repository Structure ##

| Directory | Contents                                                                                                                                                                                                                     |
|-----------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| assets    | Asset files.                                                                                                                                                                                                                 |
| bin       | Final build compilation output, categorized by OS and build config (e.g: `bin/windows/Debug/`). Does not include any assets.                                                                                                 |
| build     | CMake build directory and object files that generated during compilation.                                                                                                                                                    |
| docs      | Documentation.                                                                                                                                                                                                               |
| extlibs   | External libraries, categorized by compiler and build config (e.g: `extlibs/lib-msvc/Debug/`). For header-only library, it placed under `extlibs/headers`. For library with sources, it placed under `extlibs/libs/internal` |
| include   | Headers directory                                                                                                                                                                                                            |
| resources | Internal application resources such as Executable Icons, Application Resource (`*.rc`), Application Properties (`*.plist`) and others. This exclude resources compiled with `Compiler`.                                      |
| src       | Source code directory                                                                                                                                                                                                        |

## Project Structure ##

| Projects         | Description                                            |
|------------------|--------------------------------------------------------|
| ResourceCompiler | Compiler to compile `ControlList` into `Resources.hpp` |
| Genode           | Game Engine On Demand                                  |
| OTwo             | Core Game Client                                       |

## Compiling the Project ##

**The project does not include any copyrighted assets. You are required to provide a supported copy of the game.**

First, you need to place all assets in the `assets` folder.
- Place `Interface(1).opi`, `Playing(1).opi` and `Avatar.opa` in `assets/Image/` directory.
- Place default BGM ojm files (`BGM.ojm`, `bgEffect.ojm`, `Event.ojm`, `Planet.ojm` and `O2PlanetNPC.ojm`) in `assets/Music/` 

**Please note: the provided assets must be compatible with the supported client version.**  
You will also need to set up a local server or connect to an existing server and provide valid launch arguments to debug the project. Please refer to [Mozart.Encore](https://github.com/SirusDoma/Mozart.Encore) to learn more about setting up a local server. 

The project is now fully relies on [CMake](https://cmake.org/), and it makes use of [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html) to manage the dependencies.  
No additional setup or commands are required to build the project other than standard cmake build.

List of dependencies:
- [SFML](https://github.com/SFML/SFML)
- [boost (header-only)](https://www.boost.org/)
- [freetype2](https://freetype.org/)
- [fmt](https://github.com/fmtlib/fmt)
- [nlohmann_json](https://github.com/nlohmann/json)
- [magic_enum](https://github.com/Neargye/magic_enum)
- [kissfft](https://github.com/mborgerding/kissfft)
- [libiconv](https://www.gnu.org/software/libiconv/) (Linux and macOS)
- [compact_enc_det](https://github.com/google/compact_enc_det)
- [tinyfiledialogs](http://tinyfiledialogs.sourceforge.net)

See `CMakeList.txt` if you want to compile the executable with an icon included.

> [!Important]
> Before building a `Release` build, make sure to run `ResourceCompiler` to ensure `Resources.hpp` contains latest changes.

### CLion ###
Open the project and hit `Build` -> `Build Project`.

### Visual Studio ###
Refers to the [CMake projects in Visual Studio](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio) to open and build the project in visual studio.  

> [!WARNING]
> Do not check the visual studio project files to the git repository.  
> Make sure that only CMake files are maintained.

### CMake ###
Use the following commands to generate cmake build files:

```shell
# Generate project files with Debug configuration
$ cmake -B ./build/cmake-build-debug -DCMAKE_BUILD_TYPE=Debug

# Build the project.
$ cmake --build build/cmake-build-debug --target OTwo
```

# FAQ

## Where to get the client? is this going to work with 9you/1.8 client?
You have to find the client that matches with the `Client Version` above by yourself and I cannot help you with that.
And no, the client is not compatible to run with 1.8 client

## The game is flickering. How to fix this?
This may happen if you have multi monitors, each has different refresh rate.
This is known issue, in the meantime, you can turn off game V-Sync by pressing F10.

## Where's the ring/modifier? Why I can't set my speed when joining a room? Why some equip item slots/categories missing? Where's `OJNList.dat`? Where's (insert feature here)?
One of the project goals is to remake all major O2Jam client versions that were ever published by e-Games and Nowcom.
Starting from v3.10, because it is the earliest client that is available around the internet.

To see other client version, check the other branches/tags.

### Special notes for Pre-NX client
O2Jam v3.10 does not have ring/modifier among other things. The next iteration will bring these features.

## Can I use custom music?
Absolutely, you can!

### Special notes for Pre-NX client
client and server will perform no validation to your music list.
~~This is because client v3.10 does not have OJNList.dat implemented.~~ (This is not true: the OJNList.dat exist in `Playing(1).opi` for v3.10)

It is easy to spoof, the music. If you are playing in public space, please be mindful when playing with others.

## The music title in music selection is incorrect or gibberish, it supposed to be displayed in (Chinese/Japanese/Korea/etc). Can you fix it?
Currently, the game implemented support for legacy Unicode encodings.
However, the encoding information is not included in OJN header, so the game has to guess it. Unfortunately, this mean the decoding process is not guaranteed to be reliable all the time.

You may not notice it, but this issue also appear in the original client. Each O2Jam distribution use different set of localization implementation, which mean it may use different fonts (and probably encoding handling too).
For example, using chinese title in O2Jam client v1.8 (9you) will likely work and reliable, but the game will likely fail to display the titile for any song with Korean title. The opposite also happening with O2Jam Korea client.

## How about mods? What are the file formats the client use?
The game is a drop-in replacement of original `OTwo.exe`, which mean it can read all files that original O2Jam client can read, including `.opi`/`.opa` and `.ojn`/`.ojm` (and in the future, `OJNList.dat`).  

This mean modded `Interface(1).opi`, `Playing(1).opi` and/or `Avatar.opa` may work in this client, **as long as the files are compatible with the `Client Version` specified in the top of this README**.

The client also extend the interface/avatar asset pipeline further by utilizing json format and extremely extendible for this front, you can use this to mod or even build the scene composition from the scratch.

Further modding (such as extending game via library (.dll)) is still under planning phase. Please note that any dll injection that you use with original `OTwo.exe` **will not** work.

Also, since the game use the same network format, you can play alongside with the original game client as long as the client and server support the specified `Client Version` above.

## Can I host my own server?
Absolutely, use [Mozart.Encore](https://github.com/SirusDoma/Mozart.Encore) to host your own server or for local network play.

## Is offline mode supported?
It is not possible at the moment but it is already partially implemented inside the code. 
For now, you will have to use `Mozart.Encore` to setup local server.

## Is this Illegal?
I'm not distributing any e-Games/Nowcom/O2Jam IP. You must provide legally obtained game installer to play this game.  

The game did not contain any copyrighted materials, the source code are written from scratch and none copied from the original game binaries.
