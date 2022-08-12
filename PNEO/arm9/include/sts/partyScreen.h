/*
Pokémon neo
------------------------------

file        : partyScreen.h
author      : Philip Wellnitz
description : Consult corresponding source file.

Copyright (C) 2012 - 2022
Philip Wellnitz

This file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pokémon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <functional>
#include <vector>

#include "battle/move.h"
#include "io/strings.h"
#include "pokemon.h"
#include "sts/partyScreenUI.h"

#ifdef DESQUID
#include "fs/data.h"
#endif

namespace STS {
    class partyScreen {
      public:
        union result {
            u32 m_selectedPkmn = 0;
            struct {
                u16 m_selectedMove;
                u16 m_selectedMoveTarget; // location idx for "fly"
            };

            constexpr u8 getMark( u8 p_pos ) const {
                return ( m_selectedPkmn >> ( 4 * p_pos ) ) & 0xF;
            }
            constexpr void setMark( u8 p_pos, u8 p_mark ) {
                p_mark &= 0xf;
                m_selectedPkmn &= ~( 0xf << ( 4 * p_pos ) );
                m_selectedPkmn |= ( p_mark << ( 4 * p_pos ) );
            }
            constexpr u8 getSelectedPkmn( ) const {
                for( u8 i = 0; i < 6; ++i ) {
                    if( getMark( i ) == 1 ) { return i; }
                }
                return 255;
            }
        };

      private:
        enum choice {
            SELECT = 0,
            UNSELECT,
            STATUS,
            GIVE_ITEM,
            TAKE_ITEM,
            USE_ITEM,
            FIELD_MOVE_1,
            FIELD_MOVE_2,
            FIELD_MOVE_3,
            FIELD_MOVE_4,
            SWAP,
            DEX_ENTRY,
            CANCEL,
#ifdef DESQUID
            _DESQUID,
#endif
        };

#ifdef DESQUID
        enum desquidChoice {
            DESQUID_SPECIES = 50,
            DESQUID_STATUS,
            DESQUID_ITEM,
            DESQUID_HEAL,
            DESQUID_EGG,
            DESQUID_NATURE,
            DESQUID_IV,
            DESQUID_EV,
            DESQUID_MOVES,
            DESQUID_DUPLICATE,
            DESQUID_DELETE,

            DESQUID_CANCEL,
        };

        struct desquidItem {
            u16                        m_string;         // Desquid string id
            bool                       m_hasCounterName; // Counter values have names
            u32                        m_maxValue;       // Max value for the counter
            u32                        m_minValue;       // Min value for the counter
            std::function<u32( )>      m_currentValue;   // Callback for current value
            std::function<void( u32 )> m_counterUpdate;  // Callback when counter value changed
            std::function<std::string( u32 )>
                        m_nameForValue; // Used to obtain name for value (0 for n/a)
            std::string computeString( );
        };

        constexpr u16 getTextForDesquidChoice( const desquidChoice p_choice ) const {
            switch( p_choice ) {
            case DESQUID_SPECIES: return FS::DESQUID_STRING + 1;
            case DESQUID_STATUS: return FS::DESQUID_STRING + 2;
            case DESQUID_EGG: return FS::DESQUID_STRING + 3;
            case DESQUID_NATURE: return FS::DESQUID_STRING + 4;
            case DESQUID_ITEM: return FS::DESQUID_STRING + 5;
            case DESQUID_MOVES: return FS::DESQUID_STRING + 6;
            case DESQUID_IV: return FS::DESQUID_STRING + 7;
            case DESQUID_EV: return FS::DESQUID_STRING + 8;
            case DESQUID_DUPLICATE: return FS::DESQUID_STRING + 9;
            case DESQUID_DELETE: return FS::DESQUID_STRING + 10;
            case DESQUID_HEAL: return FS::DESQUID_STRING + 11;
            case DESQUID_CANCEL: return 330;
            }
            return 0;
        }

        std::vector<desquidItem> getDesquidItemsForChoice( const desquidChoice p_choice );
#endif
        constexpr u16 getTextForChoice( const choice p_choice ) const {
            switch( p_choice ) {
            case SELECT:
                if( !_inBattle ) {
                    return IO::STR_UI_SELECT;
                } else {
                    return IO::STR_UI_SEND_IN;
                }
            case UNSELECT: return IO::STR_UI_DESELECT;
            case STATUS: return IO::STR_UI_STATUS;
            case GIVE_ITEM: return IO::STR_UI_GIVE_ITEM;
            case TAKE_ITEM: return IO::STR_UI_TAKE_ITEM;
            case USE_ITEM: return IO::STR_UI_USE_ITEM;
            case FIELD_MOVE_1: return CHOICE_FIELD_MOVE | 0;
            case FIELD_MOVE_2: return CHOICE_FIELD_MOVE | 1;
            case FIELD_MOVE_3: return CHOICE_FIELD_MOVE | 2;
            case FIELD_MOVE_4: return CHOICE_FIELD_MOVE | 3;
            case SWAP: return IO::STR_UI_SWAP;
            case DEX_ENTRY: return IO::STR_UI_DEX_ENTRY;
            case CANCEL: return IO::STR_UI_CANCEL;
#ifdef DESQUID
            case _DESQUID: return FS::DESQUID_STRING + 0;
#endif
            }
            return 0;
        }

        std::vector<choice> _currentChoices; // Possible choices for the currently selected pkmn
        u8                  _currentChoiceSelection; // Current choice selected (SINGLE mode)

        u8       _swapSelection = 255; // Idx of pkmn to be swapped
        u8       _selectedCnt   = 0;   // Num pkmn currently selected
        pokemon* _team;                // Pkmn to display
        u8       _teamLength;          // Num Pkmn in team
        u8       _currentSelection;    // cur selected pkmn (always exactly one)
        result   _currentMarksOrMove;  // cur marked pkmn / cur selected move
        u8       _toSelect;            // num of pokemon to select
        bool     _allowMoveSelection;  // allow player to select field moves of pkmn
        bool     _allowItems;          // allow player to use items
        bool     _allowDex;            // allow player to use the pokedex
        bool     _eggSelect;           // player has to select eggs
        bool     _faintSelect;         // allow selection of fainted pkmn
        bool     _selectConfirm;       // player should confirm selection
        bool     _allowCancel;         // player may close the screen without selecting anything
        u8       _inBattle;            // number of pkmn currently in battle
        u8       _toSwap;              // Pokemon to swap

        u8             _frame;
        partyScreenUI* _partyUI;

        std::vector<std::pair<IO::inputTarget, u8>> _ranges;

        /*
         * @brief: Checks if enough pkmn are marked.
         */
        bool checkReturnCondition( );

        /*
         * @brief: Checks whether player is satisfied with the set of selected Pkmn.
         */
        bool confirmSelection( );

        /*
         * @brief: Selects the specified choice window
         */
        void selectChoice( u8 p_choice, u8 p_numChoices = 255 );

        /*
         * @brief: Makes the player choose a destination for fly from the currently
         * registered fly positions. Returns true on successful selection of a target.
         */
        bool chooseFlyTarget( );

        /*
         * @brief: Focusses the pkmn at position p_selectedIdx. Returns when pkmn leaves focus.
         */
        bool focus( );

#ifdef DESQUID
        std::vector<desquidChoice> computeDesquidChoices( );

        bool desquidWindow( desquidChoice p_choice );

        bool executeDesquidChoice( desquidChoice p_choice );

        /*
         * @brief: Opens the desquid menu for the speciefied pkmn. Returns when menu is closed.
         */
        bool desquid( u8 p_selectedIdx );
#endif

        /*
         * @brief: Changes the selection to p_selectedIdx. Unselects old selected idx.
         */
        void select( u8 p_selectedIdx );

        /*
         * @brief: Adds p_markIdx to the marked indices. Does nothing for already marked indices.
         */
        void mark( u8 p_markIdx );

        /*
         * @brief: Removes p_markIdx from the marked indices. Does nothing for unmarked indices.
         */
        void unmark( u8 p_markIdx );

        /*
         * @brief: Swaps two Pkmn.
         */
        void swap( u8 p_idx1, u8 p_idx2 );

        /*
         * @brief: Computes the possible selections for the currently selected pkmn.
         */
        void computeSelectionChoices( );

        /*
         * @brief: Executes the given choice.
         */
        bool executeChoice( choice p_choice );

        /*
         * @brief: Waits till the player touches the touch screen or presses the A or B button.
         */
        void waitForInteract( );

        /*
         * @brief: Handles touch input.
         * @returns: 1 if the player's touches had any effect; 2 if the user pressed X
         */
        u8 handleTouch( bool p_autoSel );

      public:
        /*
         * @brief: Creates a new party screen; does nothing else.
         * @param p_team: Pkmn party to display
         * @param p_teamLenth: Num Pkmn in team (max 6)
         * @param p_allowMoves: Allow to select a field move
         * @param p_allowItems: Allow to interact with items
         * @param p_allowMoves: Allow to show dex entry
         * @param p_toSelect: Number of Pkmn the player has to select. Any value > 0 makes field
         * moves unselectable
         */
        partyScreen( pokemon p_team[ 6 ], u8 p_teamLength, bool p_allowMoves = true,
                     bool p_allowItems = true, bool p_allowDex = true, u8 p_toSelect = 0,
                     bool p_confirmSelection = true, bool p_faintSelect = false,
                     bool p_eggSelect = false, bool p_allowCancel = true, u8 p_inBattle = 0,
                     u8 p_toSwap = 255 );

        ~partyScreen( );

        /*
         * @brief: Runs the party screen. Destroys anything that was previously on the screen.
         * @param p_initialSelection: initially selected Pkmn
         * @returns Either a move selected or a (bitmask) of selected indices
         */
        result run( u8 p_initialSelection = 0 );
    };
} // namespace STS
