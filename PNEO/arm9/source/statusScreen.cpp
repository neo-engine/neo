/*
Pokémon neo
------------------------------

file        : statusScreen.cpp
author      : Philip Wellnitz
description :

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

#include "sts/statusScreen.h"
#include "defines.h"
#include "io/uio.h"
#include "sound/sound.h"

namespace STS {
    statusScreen::statusScreen( pokemon* p_pokemon, bool p_allowKeyUp, bool p_allowKeyDown,
                                statusScreenUI* p_ui ) {
        _pokemon      = p_pokemon;
        _allowKeyUp   = p_allowKeyUp;
        _allowKeyDown = p_allowKeyDown;
        if( p_ui ) {
            _ui = p_ui;
        } else {
            _ui = new statusScreenUI( );
        }
    }

    void statusScreen::select( u8 p_newPage ) {
        if( _currentPage != p_newPage ) {
            _currentPage = p_newPage;
            _ui->draw( _pokemon, _currentPage );
        }
    }

    u8 statusScreen::handleTouch( ) {
        u8 change = false;
        for( auto i : _ui->getTouchPositions( ) ) {
            if( i.first.inRange( touch ) ) {
                swiWaitForVBlank( );
                change = 5;
                if( i.second == statusScreenUI::BACK_TARGET ) {
                    change = 1;
                } else if( i.second == statusScreenUI::EXIT_TARGET ) {
                    change = 2;
                } else if( i.second == statusScreenUI::NEXT_TARGET ) {
                    change = 3;
                } else if( i.second == statusScreenUI::PREV_TARGET ) {
                    change = 4;
                }
                _ui->highlightButton( i.second );

                while( touch.px || touch.py ) {
                    _ui->animate( _frame++ );
                    swiWaitForVBlank( );
                    scanKeys( );

                    if( !i.first.inRange( touch ) ) {
                        change = 0;
                        _ui->highlightButton( );
                        break;
                    }
                    touchRead( &touch );
                    swiWaitForVBlank( );
                }

                if( change ) {
                    if( change == 5 ) {
                        if( i.second < 4 ) {
                            SOUND::playSoundEffect( SFX_SELECT );
                            _currentDetailChoice = i.second;
                            _ui->showDetails( _pokemon, _currentPage, _currentDetailChoice );
                            _detailsMode = true;
                        } else if( i.second >= 50 ) {
                            select( i.second - 50 );
                        }
                        return 0;
                    } else {
                        _ui->highlightButton( );
                        return change;
                    }
                }
            }
        }
        return change;
    }

    bool statusScreen::runDetails( u8 p_initialChoice ) {
        _currentDetailChoice = p_initialChoice;
        _ui->showDetails( _pokemon, _currentPage, _currentDetailChoice );
        loop( ) {
            _ui->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_X ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                return true;
            }
            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                break;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                _currentDetailChoice = ( _currentDetailChoice + 2 )
                                       % _ui->getDetailsPageCount( _pokemon, _currentPage );
                _ui->showDetails( _pokemon, _currentPage, _currentDetailChoice );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                _currentDetailChoice = ( _currentDetailChoice
                                         + _ui->getDetailsPageCount( _pokemon, _currentPage ) - 2 )
                                       % _ui->getDetailsPageCount( _pokemon, _currentPage );
                _ui->showDetails( _pokemon, _currentPage, _currentDetailChoice );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                _currentDetailChoice = ( _currentDetailChoice + 1 )
                                       % _ui->getDetailsPageCount( _pokemon, _currentPage );
                _ui->showDetails( _pokemon, _currentPage, _currentDetailChoice );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                _currentDetailChoice = ( _currentDetailChoice
                                         + _ui->getDetailsPageCount( _pokemon, _currentPage ) - 1 )
                                       % _ui->getDetailsPageCount( _pokemon, _currentPage );
                _ui->showDetails( _pokemon, _currentPage, _currentDetailChoice );
                cooldown = COOLDOWN_COUNT;
            }
            auto tc = handleTouch( );
            if( tc == 2 ) {
                return true;
            } else if( tc == 1 ) {
                break;
            }
            swiWaitForVBlank( );
        }

        _ui->showDetails( _pokemon, _currentPage, -1 );
        return false;
    }

    statusScreen::result statusScreen::run( u8 p_initialPage ) {
        _ui->init( _pokemon, p_initialPage, _allowKeyUp, _allowKeyDown );
        _frame       = 0;
        cooldown     = COOLDOWN_COUNT;
        _currentPage = p_initialPage;

        loop( ) {
            _ui->animate( _frame++ );
            scanKeys( );
            touchRead( &touch );
            swiWaitForVBlank( );
            pressed = keysUp( );
            held    = keysHeld( );

            if( pressed & KEY_X ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                return result::EXIT;
            }
            if( pressed & KEY_B ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                return result::BACK;
            }
            if( pressed & KEY_A ) {
                SOUND::playSoundEffect( SFX_SELECT );
                if( _ui->getDetailsPageCount( _pokemon, _currentPage ) ) {
                    if( runDetails( ) ) { return result::EXIT; }
                }
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                select( ( _currentPage + 1 ) % _ui->getPageCount( ) );
                cooldown = COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                select( ( _currentPage + _ui->getPageCount( ) - 1 ) % _ui->getPageCount( ) );
                cooldown = COOLDOWN_COUNT;
            } else if( _allowKeyDown && GET_KEY_COOLDOWN( KEY_DOWN ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                return result::NEXT_PKMN;
            } else if( _allowKeyUp && GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );
                return result::PREV_PKMN;
            }

            auto tc = handleTouch( );
            if( tc == 2 ) {
                return result::EXIT;
            } else if( tc == 1 ) {
                return result::BACK;
            } else if( tc == 3 ) {
                return result::NEXT_PKMN;
            } else if( tc == 4 ) {
                return result::PREV_PKMN;
            }
            if( _detailsMode ) {
                if( runDetails( _currentDetailChoice ) ) { return result::EXIT; }
                _detailsMode = false;
            }
            swiWaitForVBlank( );
        }

        return result::EXIT;
    }

} // namespace STS
