# CXO2

**Client Version: Pre-NX (v3.10)** (3.15 or 3.38 may work, but largely untested)

<table>
  <tr>
    <td><img src="https://gist.githubusercontent.com/SirusDoma/97fa562558f8d60db047494442650ae6/raw/bc78e374cc329c8b1c292db940780eb88688434f/Screenshot1.png" width=800 height=600 /></td>
    <td><img src="https://gist.githubusercontent.com/SirusDoma/97fa562558f8d60db047494442650ae6/raw/bc78e374cc329c8b1c292db940780eb88688434f/Screenshot2.png" width=800 height=600></td>
  </tr>
  <tr>
    <td><img src="https://gist.githubusercontent.com/SirusDoma/97fa562558f8d60db047494442650ae6/raw/bc78e374cc329c8b1c292db940780eb88688434f/Screenshot5.png" width=800 height=600></td>
    <td><img src="https://gist.githubusercontent.com/SirusDoma/97fa562558f8d60db047494442650ae6/raw/bc78e374cc329c8b1c292db940780eb88688434f/Screenshot6.png" width=800 height=600></td>
  </tr>
 </table>

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
| modules   | Git submodules of related projects (e.g: `modules/Genode` for the [Genode](https://github.com/SirusDoma/Genode) game engine).                                                                                                |
| resources | Internal application resources such as Executable Icons, Application Resource (`*.rc`), Application Properties (`*.plist`) and others. This exclude resources compiled with `Compiler`.                                      |
| src       | Source code directory                                                                                                                                                                                                        |

## Project Structure ##

| Projects         | Description                                            |
|------------------|--------------------------------------------------------|
| ResourceCompiler | Compiler to compile `ControlList` into `Resources.hpp` |
| Genode           | Game Engine On Demand                                  |
| CXO2             | Core Game Client                                       |

## Compiling the Project ##

**The project does not include any copyrighted assets. You are required to provide a supported copy of the game.**

First, you need to place all assets in the `assets` folder.
- Place `Interface(1).opi`, `Playing(1).opi` and `Avatar.opa` in `assets/Image/` directory.
- Place default BGM ojm files (`BGM.ojm`, `bgEffect.ojm`, `Event.ojm`, `Planet.ojm` and `O2PlanetNPC.ojm`) in `assets/Music/` 

**Please note: the provided assets must be compatible with the supported client version.**  
When launch arguments is not provided, the game will assume to launch in offline mode. Otherwise, you will need to set up a local server or connect to an existing server and provide valid launch arguments to enable online mode. 
Please refer to [Mozart.Encore](https://github.com/SirusDoma/Mozart.Encore/tree/mozart) to learn more about setting up a local server. 

The project is now fully relies on [CMake](https://cmake.org/), and it makes use of [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html) to manage the dependencies.  
No additional setup or commands are required to build the project other than standard cmake build.

Most of the dependencies are pulled in through the [Genode](https://github.com/SirusDoma/Genode) submodule (`modules/Genode`), while vendored libraries live under `extlibs`.

List of dependencies:
- [SFML](https://github.com/SFML/SFML)
- [Boost.PFR](https://github.com/boostorg/pfr)
- [freetype2](https://freetype.org/)
- [HarfBuzz](https://github.com/harfbuzz/harfbuzz)
- [SheenBidi](https://github.com/Tehreer/SheenBidi)
- [fmt](https://github.com/fmtlib/fmt)
- [nlohmann_json](https://github.com/nlohmann/json)
- [magic_enum](https://github.com/Neargye/magic_enum)
- [dj_fft](https://github.com/jdupuy/dj_fft)
- [libiconv](https://www.gnu.org/software/libiconv/) (Linux and macOS)
- [Fontconfig](https://www.freedesktop.org/wiki/Software/fontconfig/) (Linux, optional)
- [compact_enc_det](https://github.com/google/compact_enc_det)
- [tinyfiledialogs](http://tinyfiledialogs.sourceforge.net)

See `CMakeList.txt` if you want to compile the executable with an icon included.

> [!Important]
> Before building a `Release` build, make sure to run `ResourceCompiler` to ensure `Resources.hpp` contains latest changes.

### CLion ###
Open the project and hit `Build` -> `Build Project`.

### Visual Studio ###
Refers to the [CMake projects in Visual Studio](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio) to open and build the project in visual studio.  

### CMake ###
Use the following commands to generate cmake build files:

```shell
# Generate project files with Debug configuration
$ cmake -B ./build/cmake-build-debug -DCMAKE_BUILD_TYPE=Debug

# Build the project.
$ cmake --build build/cmake-build-debug --target CXO2
```

# Documentation

## File Format ##
The file formats used by the client are documented in [docs/file-format/FileFormat.md](docs/file-format/FileFormat.md).

## FAQ ##
Refer to [docs/FAQ.md](docs/FAQ.md) for frequently asked questions.
