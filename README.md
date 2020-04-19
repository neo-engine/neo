Pokémon _neo_
===========
[![Travis](https://travis-ci.org/PH111P/perm2.svg?branch=master)](https://travis-ci.org/PH111P/perm2)

An experimental Pokémon engine for Nintendo DS.

This engine is intended to be run on real hardware, but [DeSmuME 0.9.11](http://desmume.org/) seems to be capable of emulating most of real hardware's features, too.

Features
--------

_Partially_ implemented features are in _italics_.

* [ ] A fully working Pokémon engine (at 60fps), including
    * [x] The ability to display a Pokémon's status, including detailed information about the Pokémon's moves and ribbons
      * [x] Gen 1 - Gen 5 Sprites (Pokémon Black 2 / White 2 Versions)
      * [ ] Gen 6 Sprites ([Smogon XY Sprite Project](http://www.smogon.com/forums/threads/xy-sprite-project-read-1st-post-release-v1-1-on-post-3240.3486712/))
    * [x] An in-game Pokémon storage system
    * [x] A fully working PokéDex, loading its data from the ROM's filesystem and displaying the various formes of the Pokémon
    * [x] A working battle engine inclunding
        * [x] Single, double trainer battles
        * [x] Wild Pokémon battles
        * [x] Switch Pokémon, use moves, use items
        * [ ] Move effects
        * [ ] Move animations
    * [x] A fully working bag
    * [x] The ability to load maps (for now only GBA-style maps are supported, 3D maps are planned) including
      * [ ] Events
      * [x] Wild Pokémon encounter
      * [x] Animated map tiles (at least some are animated)
      * [x] Player sprite, animated moving, surfing, cycling, and fishing
      * [x] Warps
    * [ ] Support for .midi as BGM, user customization of BGM via music stored on the micro SD card
    * [x] A real time clock (doesn't work fully on DeSmuME)
      * [ ] Overworld changes based on time
    * [x] Support for saving the progress to the micro SD (saves may break with a new version)
    * [ ] Support for communication with “the originals”
      * [ ] Playing with / importing a save from a GBA version
      * [ ] Link trading with Gen IV and Gen V games
    * [x] A clean UI
    * [ ] Dynamic difficulty
      * [ ] Adjust the encounter rate at will
      * [ ] Adjust opposing Pokémon's level (between battles)
    * [x] Support for multiple save files
      * [x] Shared PokéDex and stored Pokémon
    * [ ] Support for multiple languages
* [ ] _Easy-to-understand and ready-to-(re)use code_
    * [x] Strict and intuitive naming conventions
    * [ ] _Good documentation_
    * [x] _Modular code (strict seperation of each feature and between core and UI)_

Build requirements
------------------
* devkitARM ≥ r45
* libnds ≥ 1.5.12
* nitrofs ≥ 0.9.12
* libfat-nds ≥ 1.0.14
* dswifi ≥ 0.3.17
* maxmod ≥ 1.0.9

* gcc/g++ ≥ 4.9

* make

_More to come_, as this README.md is still under construction!
