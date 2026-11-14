/*
Pokémon neo
------------------------------

file        : battleChoice.cpp
author      : Philip Wellnitz
description : move and target selection for the player's and the opponent's slots

Copyright (C) 2012 - 2026
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

#include "bag/bagViewer.h"
#include "battle/battle.h"
#include "battle/battleField.h"
#include "battle/battleSide.h"
#include "battle/battleSlot.h"
#include "battle/battleTrainer.h"
#include "battle/battleUI.h"
#include "defines.h"
#include "dex/dex.h"
#include "fs/data.h"
#include "gen/bgmNames.h"
#include "io/animations.h"
#include "io/message.h"
#include "io/simpleWidget.h"
#include "io/strings.h"
#include "io/util.h"
#include "map/mapDrawer.h"
#include "pokemon.h"
#include "save/saveGame.h"
#include "sound/sound.h"
#include "sts/partyScreen.h"

namespace BATTLE {

    battleMoveSelection battle::chooseTarget( const battleMoveSelection& p_move ) {
        auto res       = p_move;
        res.m_moveData = FS::getMoveData( res.m_param );

        if( _policy.m_mode == BM_SINGLE ) {
            switch( res.m_moveData.m_target ) {
            case TG_ANY:
            case TG_ANY_FOE:
            case TG_ALL_FOES_AND_ALLY:
            case TG_ALL_FOES:
            case TG_RANDOM: res.m_target = { field::OPPONENT_SIDE, field::PKMN_0 }; return res;
            case TG_SELF:
            case TG_ALLY_OR_SELF:
            case TG_ALL_ALLIES: res.m_target = { field::PLAYER_SIDE, field::PKMN_0 }; return res;
            default: res.m_target = { 255, 255 }; return res;
            }
        }

        if( getBattlingPKMNCount( _policy.m_mode ) == 2 ) {
            // Double battle: User needs to choose / confirm target
            u8   possibleTargets = 0;
            u8   initialSel      = 0;
            bool hasChoice       = true;

            const u8 opp1
                = _field.getPkmn( field::OPPONENT_SIDE, field::PKMN_1 ) != nullptr ? ( 1 << 0 ) : 0,
                opp2
                = _field.getPkmn( field::OPPONENT_SIDE, field::PKMN_0 ) != nullptr ? ( 1 << 1 ) : 0,
                self = _field.getPkmn( field::PLAYER_SIDE, p_move.m_user.second ) != nullptr
                           ? ( 1 << ( 2 + p_move.m_user.second ) )
                           : 0,
                ally = _field.getPkmn( field::PLAYER_SIDE, !p_move.m_user.second ) != nullptr
                           ? ( 1 << ( 2 + !p_move.m_user.second ) )
                           : 0;
            if( !opp2 ) { initialSel++; }
            switch( res.m_moveData.m_target ) {
            case TG_ANY: possibleTargets |= opp1 | opp2 | ally; break;
            case TG_ANY_FOE: possibleTargets |= opp1 | opp2; break;
            case TG_ALLY_OR_SELF: possibleTargets |= self | ally; break;
            case TG_ALLY:
                initialSel   = ( 2 + !p_move.m_user.second );
                res.m_target = { field::PLAYER_SIDE, !p_move.m_user.second };
                possibleTargets |= ally;
                hasChoice = false;
                break;
            case TG_RANDOM: res.m_target = { field::OPPONENT_SIDE, rand( ) % 2 }; [[fallthrough]];
            case TG_SCRIPTED:
            case TG_NONE:
            case TG_SELF:
                initialSel = ( 2 + p_move.m_user.second );
                possibleTargets |= self;
                hasChoice = false;
                break;
            case TG_ALL_FOES:
                res.m_target = { field::OPPONENT_SIDE, field::PKMN_0 };
                [[fallthrough]];
            case TG_FOE_SIDE:
                possibleTargets |= opp1 | opp2;
                hasChoice = false;
                break;
            case TG_FIELD:
                possibleTargets |= opp1 | opp2 | ally | self;
                hasChoice = false;
                break;
            case TG_ALL_FOES_AND_ALLY:
                res.m_target = { field::OPPONENT_SIDE, field::PKMN_0 };
                possibleTargets |= opp1 | opp2 | ally;
                hasChoice = false;
                break;
            case TG_ALL_ALLIES:
                res.m_target = { field::PLAYER_SIDE, field::PKMN_0 };
                [[fallthrough]];
            case TG_ALLY_SIDE:
            case TG_ALLY_TEAM:
                possibleTargets |= self | ally;
                hasChoice = false;
                break;
            }

            IO::simpleChoiceBox cb
                = IO::simpleChoiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT_CANCEL );

            auto getPkmnOrDisguise = [ & ]( bool p_opp, u8 p_slot ) {
                return _field.getPkmnOrDisguise( p_opp, p_slot );
            };

            u8 rs = cb.getResult(
                [ & ]( u8 ) {
                    return _battleUI.showTargetSelection( possibleTargets, hasChoice,
                                                          getPkmnOrDisguise );
                },
                [ & ]( u8 p_selection ) {
                    _battleUI.showTargetSelection( possibleTargets, hasChoice, getPkmnOrDisguise,
                                                   p_selection );
                },
                IO::DEFAULT_TICK, initialSel );
            if( rs < 4 ) {
                // player selects a target
                if( hasChoice ) { res.m_target = { rs < 2, ( rs < 2 ) ^ ( rs % 2 ) }; }
                return res;
            }
            // cancel selection
            res.m_type = MT_CANCEL;
            return res;
        }
        res.m_type   = MT_CANCEL;
        res.m_target = { 255, 255 };
        return res;
    }

    battleMoveSelection battle::chooseAttack( u8 p_slot, bool p_allowMegaEvolution ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user              = { field::PLAYER_SIDE, p_slot };
        res.m_type              = MT_ATTACK;

        bool mega = _field.canMegaEvolve( field::PLAYER_SIDE, p_slot ) && p_allowMegaEvolution;

        // check if pkmn has move left, otherwise make it use "Struggle"
        bool canUse[ 4 ], strgl = false;
        for( u8 i = 0; i < 4; ++i ) {
            canUse[ i ] = _field.canSelectMove( field::PLAYER_SIDE, p_slot, i );
            strgl       = strgl || canUse[ i ];
        }

        if( _isMockBattle ) {
            // Always choose Tackle
            _battleUI.showAttackSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                           canUse, mega );
            auto curSel = 0;
            _battleUI.showAttackSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                           canUse, mega, curSel, res.m_megaEvolve );
            WAIT( HALF_SEC );
            _battleUI.showAttackSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                           canUse, mega, curSel = 2, res.m_megaEvolve );
            WAIT( FULL_SEC );
            res.m_param     = _field.getPkmn( field::PLAYER_SIDE, p_slot )->getMove( curSel );
            _lastMoveChoice = curSel;
            return chooseTarget( res );
        }

        if( !strgl ) {
            // pkmn will struggle
            res.m_param = M_STRUGGLE;
            return chooseTarget( res );
        }

        IO::simpleChoiceBox cb
            = IO::simpleChoiceBox( IO::choiceBox::MODE_UP_DOWN_LEFT_RIGHT_CANCEL );
        u8 curSel = _lastMoveChoice;

        loop( ) {
            u8 rs = cb.getResult(
                [ & ]( u8 ) {
                    return _battleUI.showAttackSelection(
                        _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ), canUse, mega );
                },
                [ & ]( u8 p_selection ) {
                    curSel = p_selection;
                    _battleUI.showAttackSelection(
                        _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ), canUse, mega,
                        curSel, res.m_megaEvolve );
                },
                IO::DEFAULT_TICK, curSel );

            if( rs == IO::choiceBox::BACK_CHOICE || rs == 4 ) {
                res.m_type = MT_CANCEL;
                return res;
            }
            if( rs < 4 ) {
                // player selects an attack
                if( canUse[ rs ] ) {
                    res.m_param = _field.getPkmn( field::PLAYER_SIDE, p_slot )->getMove( curSel );
                    auto tmp    = chooseTarget( res );

                    if( tmp.m_type != MT_CANCEL ) {
                        _lastMoveChoice = curSel;
                        return tmp;
                    } else {
                        res.m_param = 0;
                    }
                }
            }
            if( mega && rs == 5 ) { // Toggle Mega Evolution
                res.m_megaEvolve = !res.m_megaEvolve;
            }
        }
    }

    battleMoveSelection battle::handleMoveSelectionSelection( u8 p_slot, bool p_allowMegaEvolution,
                                                              u8 p_button ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user              = { field::PLAYER_SIDE, p_slot };
        switch( p_button ) {
        case 0: // Choose attack
            SOUND::playSoundEffect( SFX_CHOOSE );
            res = chooseAttack( p_slot, p_allowMegaEvolution );
            if( res.m_type != MT_CANCEL ) { return res; }
            break;
        case 1: { // Choose pkmn
            SOUND::playSoundEffect( SFX_CHOOSE );

            STS::partyScreen pt = STS::partyScreen(
                _playerTeam, _playerTeamSize, false, false, false, 1, false, false, false, true,
                getBattlingPKMNCount( _policy.m_mode ), p_slot );

            auto r = pt.run( p_slot );

            _battleUI.init( _field.getWeather( ), _field.getTerrain( ) );

            for( u8 side = 0; side < field::NUM_SIDES; ++side )
                for( u8 j2 = 0; j2 < getBattlingPKMNCount( _policy.m_mode ); ++j2 ) {
                    auto st = _field.getSlotStatus( side, j2 );
                    if( st == slot::status::NORMAL ) {
                        _battleUI.updatePkmn( side, j2, _field.getPkmnOrDisguise( side, j2 ) );
                    }
                }

            if( r.getSelectedPkmn( ) < 255 ) {
                res.m_type  = MT_SWITCH;
                res.m_param = r.getSelectedPkmn( );
                _battleUI.showMoveSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                             p_slot );
                return res;
            }
            break;
        }
        case 2: // Run / Cancel
            if( _isWildBattle ) {
                SOUND::playSoundEffect( SFX_CHOOSE );
                res.m_type = MT_RUN;
            } else if( p_slot ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                res.m_type = MT_CANCEL;
            } else {
                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
                break;
            }
            return res;
        case 3: { // Choose item
            if( _policy.m_distributeEXP ) {
                SOUND::playSoundEffect( SFX_CHOOSE );

                BAG::bagViewer bv
                    = BAG::bagViewer( _playerTeam, _isWildBattle ? BAG::bagViewer::WILD_BATTLE
                                                                 : BAG::bagViewer::BATTLE );
                u16 itm = bv.getItem( );

                if( itm ) {
                    auto idata = FS::getItemData( itm );

                    if( idata.m_itemType == BAG::ITEMTYPE_POKEBALL ) {
                        // Player throws a ball
                        res.m_type  = MT_CAPTURE;
                        res.m_param = itm;
                    } else if( ( idata.m_itemType & 15 ) == 2 ) {
                        // Already used
                        res.m_type  = MT_NO_OP_NO_CANCEL;
                        res.m_param = 0;
                    } else {
                        res.m_type  = MT_USE_ITEM;
                        res.m_param = itm;
                    }
                }

                _battleUI.init( _field.getWeather( ), _field.getTerrain( ) );

                for( u8 side = 0; side < field::NUM_SIDES; ++side )
                    for( u8 j2 = 0; j2 < getBattlingPKMNCount( _policy.m_mode ); ++j2 ) {
                        auto st = _field.getSlotStatus( side, j2 );
                        if( st == slot::status::NORMAL ) {
                            _battleUI.updatePkmn( side, j2, _field.getPkmnOrDisguise( side, j2 ) );
                        }
                    }

                if( itm ) {
                    _battleUI.showMoveSelection(
                        _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ), p_slot );
                    return res;
                }
            } else {
                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
                break;
            }
            break;
        }
        default: break;
        }
        res.m_type = MT_CANCEL;
        return res;
    }

    battleMoveSelection battle::getMoveSelection( u8 p_slot, bool p_allowMegaEvolution ) {
        battleMoveSelection res = NO_OP_SELECTION;
        res.m_user              = { field::PLAYER_SIDE, p_slot };

        if( !_field.canSelectMove( field::PLAYER_SIDE, p_slot ) ) {
            // pkmn is hibernating / charging
            return _field.getStoredMove( field::PLAYER_SIDE, p_slot );
        }

        auto choices = _battleUI.showMoveSelection(
            _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ), p_slot );
        u8 curSel = 0;
        _battleUI.showMoveSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ), p_slot,
                                     curSel );

        if( _isMockBattle ) {
            if( _field.getPkmn( field::PLAYER_SIDE, field::PKMN_0 )->m_stats.m_curHP * 2
                    > _field.getPkmn( field::PLAYER_SIDE, field::PKMN_0 )->m_stats.m_maxHP
                && _round <= 2 ) { // Choose tackle
                WAIT( FULL_SEC );

                SOUND::playSoundEffect( SFX_CHOOSE );
                return chooseAttack( p_slot, p_allowMegaEvolution );
            } else { // throw a poke ball
                WAIT( FULL_SEC );

                SOUND::playSoundEffect( SFX_CHOOSE );
                _battleUI.showMoveSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                             p_slot, curSel = 3 );
                WAIT( FULL_SEC );

                BAG::bagViewer bv = BAG::bagViewer( _playerTeam, BAG::bagViewer::MOCK_BATTLE );
                bv.getItem( );
                res.m_type  = MT_CAPTURE;
                res.m_param = I_POKE_BALL;

                _battleUI.init( _field.getWeather( ), _field.getTerrain( ) );

                for( u8 side = 0; side < field::NUM_SIDES; ++side )
                    for( u8 j2 = 0; j2 < getBattlingPKMNCount( _policy.m_mode ); ++j2 ) {
                        auto st = _field.getSlotStatus( side, j2 );
                        if( st == slot::status::NORMAL ) {
                            _battleUI.updatePkmn( side, j2, _field.getPkmnOrDisguise( side, j2 ) );
                        }
                    }

                _battleUI.showMoveSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                             p_slot );
                return res;
            }
        }

        GAME_LOOP( IO::DEFAULT_TICK ) {
            for( auto i : choices ) {
                if( i.first.inRange( IO::TOUCH ) ) {
                    _battleUI.showMoveSelection(
                        _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ), p_slot,
                        curSel = i.second );

                    bool bad = false;
                    while( IO::TOUCH.px || IO::TOUCH.py ) {
                        swiWaitForVBlank( );
                        if( !i.first.inRange( IO::TOUCH ) ) {
                            bad = true;
                            break;
                        }
                        scanKeys( );
                        touchRead( &IO::TOUCH );
                        swiWaitForVBlank( );
                    }
                    if( !bad ) {
                        res = handleMoveSelectionSelection( p_slot, p_allowMegaEvolution, curSel );
                        if( res.m_type != MT_CANCEL ) { return res; }
                        _battleUI.showMoveSelection(
                            _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ), p_slot );
                        _battleUI.showMoveSelection(
                            _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ), p_slot,
                            curSel );
                    }
                }
            }

            if( p_slot && ( IO::BTN_PRESSED & KEY_B ) ) {
                SOUND::playSoundEffect( SFX_CANCEL );
                res.m_type = MT_CANCEL;
                return res;
            }
            if( IO::BTN_PRESSED & KEY_A ) {
                res = handleMoveSelectionSelection( p_slot, p_allowMegaEvolution, curSel );
                if( res.m_type != MT_CANCEL ) { return res; }

                _battleUI.showMoveSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                             p_slot );
                _battleUI.showMoveSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                             p_slot, curSel );

                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_RIGHT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 ) {
                    curSel = 3;
                } else if( curSel < 3 ) {
                    curSel++;
                } else {
                    curSel = 0;
                }

                _battleUI.showMoveSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                             p_slot, curSel );

                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_LEFT ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 ) {
                    curSel = 1;
                } else {
                    curSel--;
                }

                _battleUI.showMoveSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                             p_slot, curSel );

                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
            } else if( GET_KEY_COOLDOWN( KEY_DOWN ) || GET_KEY_COOLDOWN( KEY_UP ) ) {
                SOUND::playSoundEffect( SFX_SELECT );

                if( curSel == 0 ) {
                    curSel = 2;
                } else {
                    curSel = 0;
                }

                _battleUI.showMoveSelection( _field.getPkmnOrDisguise( field::PLAYER_SIDE, p_slot ),
                                             p_slot, curSel );

                IO::BTN_COOLDOWN = IO::COOLDOWN_COUNT;
            }
        }

        return res;
    }

} // namespace BATTLE
