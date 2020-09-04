Pokémon _neo_
===========
[![Travis](https://travis-ci.org/PH111P/perm2.svg?branch=master)](https://travis-ci.org/PH111P/perm2)

An experimental Pokémon engine for Nintendo DS. For demonstration purposes only, currently
_neo_ includes some (updated) maps from Pokémon Emerald, as well as images from other
mainline Pokémon games. These are not part of _neo_.

This engine is intended to be run on real hardware, but [DeSmuME 0.9.11](http://desmume.org/) and melonDS seem to be capable of emulating most of real hardware's features, too.

Features
--------

_Partially_ implemented features are in _italics_.

* [ ] A fully working Pokémon engine (at 60fps), including
    * [x] The ability to display a Pokémon's status, including detailed information about the Pokémon's moves and ribbons
      * [x] Gen 1 - Gen 5 Sprites (Pokémon Black 2 / White 2 Versions)
      * [x] Gen 6 - Gen 8 Sprites ([Smogon XY Sprite Project](http://www.smogon.com/forums/threads/xy-sprite-project-read-1st-post-release-v1-1-on-post-3240.3486712/), [Smogon Sun/Moon Sprite Project](https://www.smogon.com/forums/threads/sun-moon-sprite-project.3577711/), [Smogon Sword/Shied Sprite Project](https://www.smogon.com/forums/threads/sword-shield-sprite-project.3647722/))
    * [x] An in-game Pokémon storage system
    * [ ] A fully working PokéDex
    * [x] A working battle engine including
        * [x] Single trainer battles
        * [x] Wild Pokémon battles
        * [x] Switch Pokémon, use moves, use items
        * [x] Move effects
        * [ ] Move animations
    * [x] A fully working bag
    * [x] The ability to load maps (for now only GBA-style maps are supported) including
      * [x] Events
      * [x] Wild Pokémon encounter
      * [x] Animated map tiles (at least some are animated)
      * [x] Player sprite, animated moving, surfing, cycling, and fishing
      * [x] Warps
    * [x] BGM and SFX using MaxMod
    * [x] A real time clock (doesn't work fully on DeSmuME)
      * [ ] Overworld changes based on time
      * [x] Different wild Pokémon encounters based on time.
    * [x] Support for save games
      * [x] Saving the progress to the micro SD (saves may break with a new version)
      * [x] Saving the progress to flash memory (highly experimental)
    * [ ] Support for communication with “the originals”
      * [ ] Playing with / importing a save from a GBA version
      * [ ] Link trading with Gen IV and Gen V games
    * [x] A clean UI
    * [x] Dynamic difficulty
      * [x] Wild Pokémon's levels scale with the story progression
      * [x] Adjust the difficulty anytime via the Options.
    * [x] Support for multiple save files
    * [x] Full support for multiple languages (English, German)
      * [x] Kana are supported font-wise, getting Kana text for story content still
        requires some work.
* [x] Easy-to-understand and ready-to-(re)use code
    * [x] Strict and intuitive naming conventions
    * [x] Documentation

Build requirements
------------------
* devkitARM ≥ r54
* libnds ≥ 1.8.1
* libfilesystem ≥ 0.9.14
* libfat-nds ≥ 1.1.5
* dswifi ≥ 0.4.2 (currently unused)
* maxmod-nds ≥ 1.0.14

* gcc/g++ ≥ 10.0

* make

Having installed the above tools, _neo_ can be built with a single `make` command. The
optional make parameter `DESQUID=1` enables the desquid mode; the parameter `NOSOUND=1` disables any sound of the complied ROM image (which makes it run much more stable on HW).

The main data for the maps, 'mons, etc, is stored in the separate `perm2-FSROOT`
repository; some data can be configured and build via tools and `csv` files found in the
`perm2-fsdata` repository.

Further helper scripts to convert images and other data can be found in the `perm2-helper`
repository.

Screenshots
-----------

##### General gameplay
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/st01.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/st02.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/gm01.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/gm02.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/gm03.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/gm04.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/gm05.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/gm06.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/gm07.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/gm08.png)

##### Pokémon status screens

![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/sts01.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/sts02.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/sts03.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/sts04.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/sts05.png)

##### The storage system

![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bx01.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bx02.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bx03.png)

##### The Bag
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bg01.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bg02.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bg03.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bg04.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bg05.png)

##### Battles
###### Wild Pokémon battles
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bt01.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bt02.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bt03.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bt04.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bt05.png)

###### Trainer battles
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/bt06.png)

#### Miscellaneous

![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/tc01.png)
![](https://github.com/PH111P/perm2/blob/master/PNEO/Screens/op01.png)
