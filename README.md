Pokémon _neo_
===========
[![Build Status](https://app.travis-ci.com/neo-engine/neo.svg?branch=main)](https://app.travis-ci.com/neo-engine/neo)

An experimental Pokémon engine for Nintendo DS. For demonstration purposes only, currently
_neo_ includes some (updated) maps from Pokémon Emerald, as well as images from other
mainline Pokémon games. These are not part of _neo_.

This engine is intended to be run on real hardware, but [DeSmuME 0.9.11](http://desmume.org/) and melonDS seem to be capable of emulating most of real hardware's features, too.

Features
--------

_Partially_ implemented features are in _italics_.

* [x] A working Pokémon engine (at 60fps), including
    * [x] A status view for single Pokémon as well as the player's party, including detailed information about the Pokémon's moves and ribbons
      * [x] Gen 1 - Gen 5 Sprites (Pokémon Black 2 / White 2 Versions)
      * [x] Spinda support
      * [x] Gen 6 - Gen 8 Sprites ([Smogon XY Sprite Project](http://www.smogon.com/forums/threads/xy-sprite-project-read-1st-post-release-v1-1-on-post-3240.3486712/), [Smogon Sun/Moon Sprite Project](https://www.smogon.com/forums/threads/sun-moon-sprite-project.3577711/), [Smogon Sword/Shied Sprite Project](https://www.smogon.com/forums/threads/sword-shield-sprite-project.3647722/))
      * [x] Slight color variations of Pokémon based on their PV.
    * [x] An in-game Pokémon storage system
    * [x] A working PokéDex, listing for each Pokémon
        * [x] A flavor text
        * [x] Types, simplified base stats, height, and weight
        * [ ] Different forms, if any
        * [ ] Locations where (and when) the Pokémon can be found in-game
    * [x] A working battle engine including
        * [x] Single and double trainer battles
        * [ ] Multi trainer battles
        * [x] Wild Pokémon battles
        * [x] Switch Pokémon, use moves, use items
        * [x] Move effects, item effects, ability effects
        * [ ] Move animations
    * [x] A fully working bag
    * [x] A working map engine (for now only GBA-style maps are supported) including
        * [x] Events, Warps
        * [x] Wild Pokémon encounters
        * [x] Pokémon Trainers challenging the player
        * [x] Animated map tiles, changing tiles based on story progress
        * [x] Player sprite, animated moving, surfing, cycling, and fishing
        * [x] PKMN following the player
        * [x] Weather and other overlays (flash, sandstorm, etc)
        * [x] Field moves such as Surf, Waterfall, Rock Climb, Cut, Strength, Fly, Dive,
          etc
        * [ ] Currently unsupported field moves: Whirlpool, Defog
        * [x] Mach Bike, Acro Bike, Fishing Rods, PokéRadar, Escape Rope, Honey
    * [x] BGM and SFX including
        * [x] An in-game jukebox
        * [x] Compatibility with official NDS game music (using `.sseq`, `.sbnk`, and
          `.swar` files for BGM playback).
    * [x] A real time clock
        * [x] Overworld changes based on time (map palette akin to DNS)
        * [x] Different wild Pokémon encounters based on time.
        * [x] Map events based on day time
    * [x] Support for (multiple) save games
        * [x] Saving the progress to the micro SD (works only on flash cards)
        * [x] Saving the progress to flash memory (works only in emulators)
    * [ ] Support for communication with “the originals”
        * [ ] Playing with / importing a save from a GBA version
        * [ ] Link trading with Gen IV and Gen V games
    * [x] A clean UI
    * [x] Dynamic difficulty
        * [x] Wild Pokémon's levels scale with the story progression
        * [x] Adjust the difficulty anytime via the Options.
    * [x] Full support for multiple languages (English, German)
        * [x] Kana are supported font-wise, getting Kana text for story content still requires some work.

* [x] Easy-to-understand and ready-to-(re)use code
    * [x] Strict and intuitive naming conventions
    * [x] Documentation

Build requirements
------------------
* devkitARM ≥ r57
* libnds ≥ 1.8.2
* libfilesystem ≥ 0.9.14
* libfat-nds ≥ 1.1.5
* dswifi ≥ 0.4.2 (currently unused)

* gcc/g++ ≥ 11.2 (for ndstool)

* make

Having installed the above tools, _neo_ can be built with a single `make` command.
See the section below for optional compilation parameters.

The main data for the maps, 'mons, etc, is stored in the separate `perm2-FSROOT`
repository; some data can be configured and build via tools and `csv` files found in the
`perm2-fsdata` repository.

Further helper scripts to convert images and other data can be found in the `perm2-helper`
repository.

Compilation Parameters
----------------------

All of the following parameter can be specified as environment variables or as parameters
to make; the exact (non-empty) value of a variable is unused.

* `DESQUID` Enable _desquid mode_. Adds a desquid menu to the party screen, allowing for
  an easy way to edit and/or test Pokémon. Also adds a global dequid menu to `SELECT`;
  shows the current location and other desquid output on `L`, and enables a
  walk-through-walls mode while pressing `R`.
* `NOSOUND` Disable all sound output.
* `FLASHCARD` Optimize for flashcards. Uses `argv` parameters to write save game files
  (instead of writing directly to CARD memory). Typically, `FLASHCARD` builds cannot
  create/load save games in an emulator.

Screenshots
-----------

More screenshots are available in the `Screens` subdirectory.

##### General gameplay
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/st01.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/st02.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm21.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm01.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm02.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm03.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm04.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm05.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm07.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm08.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm09.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm10.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm11.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm12.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm13.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm14.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm15.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm16.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm17.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm18.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm23.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm24.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm26.png)

##### Pokémon status screens

![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/sts01.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/sts02.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/sts03.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/sts04.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/sts05.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/sts06.png)

##### The storage system

![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/gm06.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bx01.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bx02.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bx03.png)

##### The Bag
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bg01.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bg02.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bg03.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bg04.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bg05.png)

##### The PokéDex
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/dx01.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/dx02.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/dx03.png)

##### Battles
###### Wild Pokémon battles
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bt01.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bt02.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bt03.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bt07.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bt08.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bt09.png)

###### Trainer battles
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bt06.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bt05.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/bt10.png)

#### Miscellaneous

![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/tc01.png)
![](https://github.com/neo-engine/neo/blob/main/PNEO/Screens/op01.png)
