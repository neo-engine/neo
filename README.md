Pokémon Emerald Version 2
=========================
[![Travis](https://travis-ci.org/PH111P/perm2.svg?branch=master)](https://travis-ci.org/PH111P/perm2)

There's still (very) much to do, but when it's complete, it's a recode of Pokémon for Nintendo DS.

This game is intended to be played on real hardware, but [DeSmuME 0.9.11](http://desmume.org/) seems to be capable of emulating most of real hardware's features, too.

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
    * [ ] Support for multiple save files and “New Game+”
      * [ ] Shared PokéDex and stored Pokémon
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

Some screens
------------

##### General gameplay
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/st01.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/st02.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/gm01.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/gm02.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/gm03.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/gm04.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/gm05.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/gm06.png)

##### Pokémon status screens

![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts01.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts02.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts03.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts04.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts05.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts06.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts07.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts08.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts09.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/sts10.png)

##### The storage system

![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bx01.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bx02.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bx03.png)

##### The PokéDex
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/dx01.png)

##### The Bag
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg01.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg02.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg03.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg04.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg05.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg06.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg07.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg08.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg09.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg10.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg11.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg12.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg13.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg14.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bg15.png)

##### Battles
###### Wild Pokémon battles
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt01.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt02.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt03.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt04.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt05.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt06.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt07.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt08.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt09.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt10.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt11.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt12.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt13.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt14.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt15.png)

###### Trainer battles
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt16.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt17.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt18.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt19.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt20.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt21.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt22.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt23.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt24.png)
![](https://github.com/PH111P/perm2/blob/master/P-Emerald_2/Screens/bt25.png)

_More to come_, as this README.md is still under construction!
