/*
Pok\xe9mon neo
------------------------------

file        : strings.cpp
author      : Philip Wellnitz
description :

Copyright (C) 2012 - 2020
Philip Wellnitz

tHis file is part of Pokémon neo.

Pokémon neo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pokémon neo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pok\xe9mon neo.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "defines.h"
#include "saveGame.h"

// ae \xe4
// e´ \xe9
// oe \xf6
// ue \xfc

// AE \xc4
// E´ \xc9
// OE \xd6
// UE \xdc
// ss \xdf
// { (male)
// } (female)
//

// ,, [131]
// '' [129]
// ´´ \"

const char* SAVE::EPISODE_NAMES[ MAX_SPECIAL_EPISODES + 1 ][ LANGUAGES ]
    = { { "Dev's Heaven", "Dev's Heaven" } };

const char* LANGUAGE_NAMES[ LANGUAGES ] = { "English", "Deutsch" };

#ifdef DESQUID
const char* const DESQUID_STRINGS[ MAX_DESQUID_STRINGS ][ LANGUAGES ] = {
    { "Desquid" },
    { "Basic Data" },
    { "Status" },
    { "Egg Status" },
    { "Nature" },

    { "HP/PP" },
    { "Moves/Item" },
    { "IV" },
    { "EV" },
    { "Duplicate" },

    // 10

    { "Delete" },
    { "Heal" },
    { "Species: %s" },
    { "Forme: %s" },
    { "Ability: %s" },

    { "Sleep" },
    { "Poison" },
    { "Burn" },
    { "Freeze" },
    { "Paralyzed" },

    // 20

    { "Toxic" },
    { "Level" },
    { "EXP" },
    { "Shininess" },
    { "Is Egg?" },

    { "Steps/Happiness" },
    { "Obt. at %s" },
    { "Hatd at %s" },
    { "OT Id" },
    { "OT SId" },

    // 30

    { "Nature: %s" },
    { "%2s Attack" },
    { "%2s Defense" },
    { "%2s Special Atk" },
    { "%2s Special Def" },

    { "%2s Speed" },
    { "HP" },
    { "Attack" },
    { "Defense" },
    { "Special Attack" },

    // 40

    { "Special Defense" },
    { "Speed" },
    { "%s" },
    { "Held: %s" },
    { "PP(%s)" },

    { "Fateful Enc." },
    { "PKMN Spawn" },
    { "Item Spawn" },
    { "View Boxes" },
    { "Tokens of god-being|" },

    // 50

    { "RND Badge" },
    { "Init PKMN" },
    { "Pok\xe9Mart" },
};

#endif

const char* const MONTHS[ 12 ][ LANGUAGES ]
    = { { "Jan.", "Jan." }, { "Feb.", "Feb." }, { "Mar.", "M\xe4r." }, { "Apr.", "Apr." },
        { "May", "Mai" },   { "June", "Juni" }, { "July", "Juli" },    { "Aug.", "Aug." },
        { "Sep.", "Sep." }, { "Oct.", "Okt." }, { "Nov.", "Nov." },    { "Dec.", "Dez." } };

const char* const ACHIEVEMENTS[ MAX_ACHIEVEMENTS ][ LANGUAGES ] = {
    { "", "" },
    { "Won the Stone Badge on %s", "Steinorden erhalten am %s" },
    { "Won the Knuckle Badge on %s", "Kn\f6"
                                     "chelorden erhalten am %s" },
    { "Won the Dynamo Badge on %s", "Dynamo-Orden erhalten am %s" },
    { "Won the Heat Badge on %s", "Hitzeorden erhalten am %s" },
    { "Won the Balance Badge on %s", "Balanceorden erhalten am %s" },
    { "Won the Feather Badge on %s", "Federorden erhalten am %s" },
    { "Won the Mind Badge on %s", "Mentalrden erhalten am %s" },
    { "Won the Rain Badge on %s", "Schauerorden erhalten am %s" },
    { "Entered the Hall of Fame on %s", "Einzug in die Ruhmeshalle am %s" },
    { "Won the S. Knowledge Sym. on %s", "S-Kenntnissym. erhalten am %s" },
    { "Won the G. Knowledge Sym. on %s", "G-Kenntnissym. erhalten am %s" },
    { "Won the S. Guts Symbol on %s", "S-Grobsymbol erhalten am %s" },
    { "Won the G. Guts Symbol on %s", "G-Grobsymbol erhalten am %s" },
    { "Won the S. Tactics Symbol on %s", "S-Taktiksymbol erhalten am %s" },
    { "Won the G. Tactics Symbol on %s", "G-Taktiksymbol erhalten am %s" },
    { "Won the S. Luck Symbol on %s", "S-Lossymbol erhalten am %s" },
    { "Won the G. Luck Symbol on %s", "G-Lossymbol erhalten am %s" },
    { "Won the S. Spirits Symbol on %s", "S-Seelensymbol erhalten am %s" },
    { "Won the G. Spirits Symbol on %s", "G-Seelensymbol erhalten am %s" },
    { "Won the S. Brave Symbol on %s", "S-Bravursymbol erhalten am %s" },
    { "Won the G. Brave Symbol on %s", "G-Bravursymbol erhalten am %s" },
    { "Won the S. Ability Symbol on %s", "S-Anlagesymbol erhalten am %s" },
    { "Won the G. Ability Symbol on %s", "G-Anlagesymbol erhalten am %s" },
    { "Battle Frontier cleared on %s", "Kampfzone bezwungen am %s" },
};

const char* const BADGENAME[ MAX_BADGENAMES ][ LANGUAGES ] = {
    { "Stone Badge", "den Steinorden" },
    { "Knuckle Badge", "den Kn\f6"
                       "chelorden" },
    { "Dynamo Badge", "den Dynamo-Orden" },
    { "Heat Badge", "den Hitzeorden" },
    { "Balance Badge", "den Balanceorden" },
    { "Feather Badge", "den Federorden" },
    { "Mind Badge", "den Mentalrden" },
    { "Rain Badge", "den Schauerorden" },
    { "Silver Knowledge Symbol", "das Silber-Kenntnissymbol" },
    { "Gold Knowledge Symbol", "das Gold-Kenntnissymbol" },
    { "Silver Guts Symbol", "das Silber-Grobsymbol" },
    { "Gold Guts Symbol", "das Gold-Grobsymbol" },
    { "Silver Tactics Symbol", "das Silber-Taktiksymbol" },
    { "Gold Tactics Symbol", "das Gold-Taktiksymbol" },
    { "Silver Luck Symbol", "das Silber-Lossymbol" },
    { "Gold Luck Symbol", "das Gold-Lossymbol" },
    { "Silver Spirits Symbol", "das Silber-Seelensymbol" },
    { "Gold Spirits Symbol", "das Gold-Seelensymbol" },
    { "Silver Brave Symbol", "das Silber-Bravursymbol" },
    { "Gold Brave Symbol", "das Gold-Bravursymbol" },
    { "Silver Ability Symbol", "das Silber-Anlagesymbol" },
    { "Gold Ability Symbol", "das Gold-Anlagesymbol" },
};

const char* const MAP_STRINGS[ MAX_MAP_STRINGS ][ LANGUAGES ] = {
    { "Meteor Falls", "Meteorf\xe4lle" },
    { "\x05 Fallarbor Town\nRoute 113 \x08", "\x05 Laubwechselfeld\nRoute 113 \x08" },
    { "Fallarbor Town", "Laubwechselfeld" },
    { "Battle Tent", "Kampfzelt" },
    { "Move Reminder's House", "Haus des Attackenmaniac" },

    { "Fossil Maniac's House", "Haus des Fossilienmaniac" },
    { "Lanette's House", "Lanettes Haus" },
    { "Littleroot Town\n[129]"
      "A town that can't be shaded any hue.\"",
      "Wurzelheim" },
    { "Prof. Birch's Lab", "Prof. Birks Labor" },
    { "[PLAYER]'s House", "Haus von [PLAYER]" },

    // 10

    { "Prof. Birch's House", "Haus von Prof. Birk" },
    { "Prof. Birch's House", "Haus von Prof. Birk" },
    { "Mom: Welcome to Littleroot Town, honey.\r"
      "How do you like it?\nThis is our new home.\r"
      "It has a quaint feel, but it seems to be an\neasy place to live, don't you think?\r"
      "And you get your own room, [PLAYER]!\nLet's go inside.",
      "Mutter: Willkommen in Wurzelheim, Schatz.\r"
      "Wie gef\xe4llt es dir?\nDas ist unser neues zu Hause." },
    { "Route 101\n\x06 Oldale Town", "Route 101\n\x06 Rosaltstadt" },
    { "Wild Pok\xe9mon will jump out at you in tall grass.\r"
      "If you want to catch Pok\xe9mon, you have to go\ninto the tall grass and search.",
      "" },

    { "If Pok\xe9mon get tired, take them to\na Pok\xe9mon Center.\r"
      "There's a Pok\xe9mon Center in Oldale Town\nright close by.",
      "" },
    { "Mom: See, [PLAYER]?\nIsn't it nice in here, too?", "" },
    { "[129]Rejuvenate your tired partners\"\nPok\xe9mon Center", "" },
    { "Oldale Town\n[129]Where things start of scarce.\"", "Rosaltstadt" },
    { "[129]Select items for your convenience\"\nPok\xe9mon Mart", "" },

    // 20

    { "I want to take a rest,\nso I'm saving my progress.", "" },
    { "My Pok\xe9mon is staggeringly tired|\nI should have brought a Potion|", "" },
    { "Mom: [PLAYER].\r"
      "Please check out your new PC.\nIt's a gift from Dad after all.",
      "" },
    { "Route 103\n\x07 Oldale Town", "Route 103\n\x07 Rosaltstadt" },
    { "[CRY:288]Fugiiih!", "[CRY:288]Fugiiih!" },

    { "[CRY:288]Huggoh, uggo uggo|", "[CRY:288]Huggoh, uggo uggo|" },
    { "Dad might like this program.\n| | | | | | | | | | | | |\r"
      "Better get going!",
      "" },
    { "Pok\xe9mon Centers are great!\r"
      "You can use their services as much\nas you like, and it's all for free.\r"
      "You never have to worry!",
      "" },
    { "It's a Nintendo GameCube.", "Eine Nintendo GameCube." },
    { "[PLAYER] flipped open the notebook.\r"
      "Adventure Rule No.1\nOpen the menu with X or START.\r"
      "Adventure Rule No.2\nRecord your progress with Save.\r"
      "The remaining pages are blank|",
      "" },

    // 30

    { "Pok\xe9mon magazines!\n[129]The Unown - Hoenn Edition\"|\n"
      "[129]How to Train Your Bagon\"|\nAll of them are somewhat boring|",
      "Pok\xe9mon-Magazine!\n[131]Icognito in Hoenn[129]|\n"
      "[131]Kindwurmz\xe4hmen leicht gemacht[129]|\nKeines wirkt besonders ansprechend|" },
    { "I'm sorry, but we're currently\nrenovating the top floor.",
      "Es tut mir Leid, aber wir bauen\ndas Obergeschoss gerade um." },
    { "That PC in the corner there is for any\nPok\xe9mon Trainer to use.\r"
      "Naturally, that means you're welcome\nto use it, too.",
      "" },
    { "[PLAYER] booted up the PC.", "[PLAYER] schaltet den PC ein." },
    { "A message from Dad!\r"
      "Dad: Hi [PLAYER]! Welcome to our new home.\r"
      "I'm busy right now, so I can't\nwelcome you in person.\r"
      "But we should meet nevertheless!\r"
      "So could you please come\nand visit me in my Gym in Petalburg City?\r"
      "Ah, this Potion may come in handy\nfor your journey.",
      "" },

    { "When Pok\xe9mon battle, they eventually\nlevel up and become stronger.", "" },
    { "If the Pok\xe9mon with you become stronger,\nyou'll be able to go farther away from here.",
      "" },
    { "When a Pok\xe9mon battle starts, the one at the\ntop-left of the list goes out first.\r"
      "So, when you get more Pok\xe9mon in your party,\ntry switching around their order.\r"
      "It could give you an advantage.",
      "" },
    { "There is another message.\r"
      "???: Greetings [PLAYER], I hope you are having fun.\nThis gift was delivered just for you.",
      "Es gibt eine weitere Nachricht.\r"
      "???: Sei gegr\xfc\xdft, [PLAYER]. Dieses Geschenk ist f\xfcr dich." },
    { "Hello and welcome to the Pok\xe9mon Center.\r"
      "We restore your tired Pok\xe9mon to full health.",
      "Willkommen im Pok\xe9mon-Center.\r"
      "Wir heilen deine Pok\xe9mon und\nmachen sie wieder fit." },

    // 40

    { "Mom: [PLAYER], how do you like your new room?\r"
      "Good! Everything's put away neatly!\r"
      "They finished moving everything in\ndownstairs, too!\r"
      "Pok\xe9mon movers are so convenient!",
      "" },
    { "The clerk says they're all sold out.\nI can't buy any Pok\xe9 Balls.", "" },
    { "If a Pok\xe9mon gets hurt and loses its HP and\nfaints, it won't be able to battle.\r"
      "To prevent your Pok\xe9mon from fainting,\nrestore its HP with a Potion.",
      "" },
    { "Would you like to rest your Pok\xe9mon?", "OK. Wir ben\xf6tigen deine Pok\xe9mon." },
    { "Mom: Oh! [PLAYER], [PLAYER]!\nQuick! Come quickly!", "" },

    { "Mom: Look it's Petalburg Gym!\nMaybe Dad will be on!"
      "" },
    { "We hope to see you again!", "Komm jederzeit wieder vorbei!" },
    { "Mom: Oh| It's over.\r"
      "I think Dad was on, but we missed him. Too bad.\r"
      "Oh yes.\nOne of Dad's friends lives in town.\r"
      "Prof. Birch is his name.\r"
      "He lives right next door, so you should\ngo over and introduce yourself.",
      "" },
    { "Okay, I'll take your Pok\xe9mon for a second.",
      "Okay, ich nehme deine Pok\xe9mon für einen Moment in meine Obhut." },
    { "Thank you for waiting.\r"
      "We've restored your Pok\xe9mon to full health.\r"
      "We hope to see you again.",
      "Vielen Dank.\r"
      "Deine Pok\xe9mon sind wieder top fit.\r"
      "Komm jederzeit wieder vorbei!" },

    // 50

    { "The mover's Pok\xe9mon do all the work of\nmoving us in and cleaning up after.\r"
      "This is so convenient!\r"
      "[PLAYER], your room is upstairs.\nGo check it out, dear!\r"
      "Dad bought you a new PC to mark our move\nhere. Don't forget to check it out.",
      "" },
    { "There's an e-mail from the Pok\xe9mon\nTrainers' School.\r"
      "| | | | | | |\r"
      "A Pok\xe9mon may learn up to four moves.\r"
      "A trainer's expertise is tested on the\nmove sets chosen for Pok\xe9mon.\r"
      "| | | | | | |",
      "" },
    { "Mom: See you, honey!", "" },
    { "Prof. Birch spends days in his lab studying,\nthen he'll suddenly go out\n"
      "in the wild to do more research|\r"
      "When does Prof. Birch spend time at home?",
      "" },
    { "Interviewer: |this report from in front of\nPetalburg Gym.", "" },

    { "", "" },
    { "Using a PC you can send even items!\n"
      "The power of science is staggering!",
      "" },
    { "Um, um, um!", "" },
    { "If you go outside and go in the grass,\nwild Pok\xe9mon will jump out!\r"
      "It's dangerous if you don't have your\nown Pok\xe9mon!",
      "" },
    { "Oh, hello. And you are?\r"
      "| | | | | | | | |\n| | | | | | | | |\r"
      "Oh, you're [PLAYER],\nour new next-door neighbor! Hi!\r"
      "We have a kid about the same age as you.\r"
      "Our kid was excited about\nmaking a new friend.\r"
      "Our kid is upstairs, I think.",
      "" },

    // 60

    { "Like child, like father.\r"
      "My husband is as wild about\nPok\xe9mon as our child.\r"
      "If my husband is not at his Lab, he's likely scrabbling about in grassy places.",
      "" },
    { "It's crammed with books on Pok\xe9mon.", "" },
    { "It's a PC used for research.\nBetter not mess around with it.", "" },
    { "It's a serious-looking machine.\nIt must be used for research.", "" },
    { "Hunh? Prof. Birch?\r"
      "The Prof's away on fieldwork.\nErgo, he isn't here.\r"
      "Oh, let me explain what fieldwork is.\r"
      "It is to study things in the natural\nenvironment, like fields of mountains,\n"
      "instead of a laboratory.\r"
      "The Prof isn't one for doing desk work.\nHe's the type of person who would\n"
      "rather go outside and experience things\nthan read about them here.",
      "" },

    { "Hi, neighbor!\r"
      "Do you already have your own Pok\xe9mon?",
      "" },
    { "Let me explain again what fieldwork is.\r"
      "It is to study things in the natural\nenvironment, like fields of mountains,\n"
      "instead of a laboratory.\r"
      "The Prof isn't one for doing desk work.\nHe's the type of person who would\n"
      "rather go outside and experience things\nthan read about them here.",
      "" },
    { "Oh, you're [PLAYER], aren't you?\nMoved in next door, right?\r"
      "My name's [RIVAL]. So hi, neighbor.\r"
      "Huh? Hey, [PLAYER],\ndon't you have a Pok\xe9mon?\r"
      "Dad, Prof. Birch, said that our new\nnext-door neighbor is a Gym Leader's kid|\r"
      "So, I assumed you would already have\nsome cool, strong Pok\xe9mon|\r"
      "Do you want me to go catch you one?\r"
      "Ah, I forgot|\r"
      "I'm supposed to go help my dad\ncatch some wild Pok\xe9mon.\r"
      "Some other time, okay?",
      "" },
    { "It's [RIVAL]'s Pok\xe9 Ball!\r"
      "Better leave it right where it is.",
      "" },
    { "Pok\xe9mon fully restored|\nItems all packed, and|", "" },

    // 70

    { "That [RIVAL]!\r"
      "I guess our child is too busy with Pok\xe9mon\nto notice that you came to visit|",
      "" },
    { "It's a book that is too hand to read.", "" },
    { "It's a half-empty pot of warm coffee.", "" },
    { "It's a half-full pot of cold coffee.", "" },
    { "Hey!\nYou|\r"
      "Who are you?",
      "" },

    { "Um, hi!\r"
      "There are scary Pok\xe9mon outside!\nI can hear their cries!\r"
      "I want to go see what's going on,\nbut I don't have any Pok\xe9mon|\r"
      "Can you go see what's happening for me?",
      "" },
    { "H-Help me!", "" },
    { "Hello! You over there!\nPlease! Help!\r"
      "In my bag!\nThere's a Pok\xe9 Ball!",
      "" },
    { "Prof. Birch: Whew|\r"
      "I was in the tall grass studying wild\nPok\xe9mon when I was jumped.\r"
      "You saved me. Thanks a lot!\r"
      "Oh?\r"
      "Hi, you're [PLAYER]!\r"
      "This is not the place to chat, so come by my\nPok\xe9mon Lab later, okay?",
      "" },
    { "Prof. Birch: So, [PLAYER].\r"
      "I've heard so much about you\nfrom your father.\r"
      "I've heard that you don't have\nyour own Pok\xe9mon yet.\r"
      "But the way you battled earlier,\nyou pulled it off with aplomb!\r"
      "I guess you have your father's blood\nin your veins after all!\r"
      "Oh yes. As thanks for rescuing me, I'd like\nyou to have the Pok\xe9mon you used earlier.",
      "" },

    // 80

    { "[PLAYER] received [TEAM:0].", "[PLAYER] erhielt [TEAM:0]." },
    { "Would you like to give a nickname to [TEAM:0]?", "" },
    { "Prof. Birch: If you work at Pok\xe9mon and gain\nexperience, I think you'll make an\n"
      "extremely good Pok\xe9mon Trainer.\r"
      "My kid, [RIVAL], is also studying Pok\xe9mon\nwhile helping me out.",
      "" },
    { "Prof. Birch: Oh, don't be that way.\nYou should go meet my kid.", "" },
    { "Prof. Birch: Great!\n[RIVAL] should be happy, too.\r"
      "Get [RIVAL] to teach you what it means\nto be a Pok\xe9mon Trainer.",
      "" },

    { "Prof. Birch: [RIVAL]?\nGone home, I think.\r"
      "Or maybe that kid's scrabbling around in\ntall grass again somewhere|\r"
      "If you or your Pok\xe9mon get tired,\nyou should get some rest at home.",
      "" },
    { "Prof. Birch is studying the habitats and\ndistribution of Pok\xe9mon.\r"
      "The Prof enjoys [RIVAL]'s help, too.\nThere's a lot of love there.",
      "" },
    { "You saved Prof. Birch!\nI'm so glad!", "" },
    { "Oh, [RIVAL] went out to Route 103\njust a little while ago.\r"
      "Like father, like child.\n[RIVAL] can't stay quietly at home.",
      "" },
    { "Mom: How are you doing, [PLAYER]?\nYou look a little tired.\r"
      "I think you should rest a bit.",
      "" },

    // 90

    { "Mom: Take care, honey!.", "" },
    { "Aaaah! Wait!\nPlease don't come in here.", "" },
    { "I just discovered the footprints\nof a rare Pok\xe9mon!\r"
      "Wait until I finish sketching them, okay?",
      "" },
    { "", "" },
    { "Hi!\nI work at a Pok\xe9mon Mart.\r"
      "What is a Pok\xe9mon Mart you may ask?\r"
      "It is a building just as the one right behind me.\nJust look for our blue roof.\r"
      "We sell a variety of goods including\nPok\xe9 Balls for catching Pok\xe9mon.\r"
      "Here, I'd like you to have these\nPotions as promotional items.",
      "" },

    { "A Potion can be used anytime, so it can be\neven more useful than a Pok\xe9mon Center.",
      "" },
    { "[RIVAL]: Okay, so it's this one and that one\nthat live on Route 103|", "" },
    { "Hey, it's [PLAYER]!\r"
      "|Oh, yeah, Dad gave you a Pok\xe9mon.\r"
      "Since we're here, how about a little battle?\r"
      "I'll teach you what being a\nPok\xe9mon Trainer's about!",
      "" },
    { "[RIVAL]: I think I get it. I think I know\nwhy my dad has his eye out for you now.\r"
      "Look, your Pok\xe9mon already likes you, even though you just got it.\r"
      "[PLAYER], I get the feeling that you could\nbefriend any Pok\xe9mon with ease.\r"
      "| | |\r"
      "We should head back to the lab.",
      "" },
    { "", "" },

    // 100
    { "I think it's wonderful for people to\ntravel with Pok\xe9mon.\r"
      "But you should go home every so often\nto let you mother know you're okay.\r"
      "She might not say it, but I'm sure she\nworries about you, [PLAYER].",
      "" },
    { "Prof. Birch: Oh, welcome back, [PLAYER]!\r"
      "I heard you beat [RIVAL] on your first try.\nThat's excellent!\r"
      "[RIVAL]'s been helping with my research\nfor a long time.\r"
      "[RIVAL] has an extensive history as a\nPok\xe9mon Trainer already.\r"
      "That reminds me, [PLAYER].\r"
      "Here, I ordered this for my research,\nbut I think you should have this Pok\xe9"
      "Dex.",
      "" },
    { "[PLAYER] received the Pok\xe9"
      "Dex.",
      "" },
    { "Prof. Birch: The Pok\xe9"
      "Dex is a high-tech tool\nthat automatically makes a record\n"
      "of any Pok\xe9mon you meet or catch.\r"
      "My kid, [RIVAL], goes everywhere with it.\r"
      "Whenever my kid catches a rare Pok\xe9mon\nand records its data in the Pok\xe9"
      "Dex,\n"
      "why, [RIVAL] looks for me while I'm out\ndoing fieldwork, and shows me.",
      "" },
    { "[RIVAL]: Huh|\nSo you got a Pok\xe9"
      "Dex, too.\r"
      "Well then, here.\nI'll give you these.",
      "" },

    { "[RIVAL]: You know it's more fun to have a\nwhole bunch of Pok\xe9mon.\r"
      "I'm going to explore all over the place to find\ndifferent Pok\xe9mon.\r"
      "If I find any cool Pok\xe9mon, you bet I'll try\nto get them with Pok\xe9 Balls.",
      "" },
    { "[RIVAL]: Where should I look for Pok\xe9mon\nnext|", "" },
    { "Prof. Birch: Countless Pok\xe9mon await you!\r"
      "Argh, I'm getting the itch to get out and\ndo fieldwork again!",
      "" },
    { "Mom: Wait, [PLAYER]!", "" },
    { "Mom: [PLAYER]! [PLAYER]! Did you introduce\nyourself to Prof. Birch?\r"
      "Oh! What an adorable Pok\xe9mon!\nYou got it from Prof. Birch. How nice!\r"
      "You're your father's child, all right.\nYou look good together with Pok\xe9mon!\r"
      "| | | | | | | | | |\n| | | | | | | | | |\r"
      "To think that you have your\nvery own Pok\xe9mon now|\r"
      "Your father will be overjoyed.\r"
      "|But please be careful.\nIf anything happens, you can come home.\r"
      "Go on, go get them, honey!",
      "" },

    // 110

    { "Are you going to catch Pok\xe9mon?\nGood luck!", "" },
    { "I finished sketching te footprints of a rare Pok\xe9mon.\r"
      "But it turns out they were only\nmy own footprints|",
      "" },
    { "Mom: Did you introduce yourself to\nProf. Birch?", "" },
    { "[PLAYER], don't you think it might\nbe a good idea to go see [RIVAL]?", "" },
    { "Route 102\nOldale Town \x08", "Route 102\nRosaltstadt \x08" },

    { "Route 102\n\x05 Petalburg City", "Route 102\n\x05 Bl\xfctenburg City" },
    { "Petalburg City\n[129]Where people mingle with nature.\"", "Bl\xfctenburg City\n[131][129]" },
    { "Wally's House", "Haus von Heiko" },
    { "I'm|not very tall, so I sink\nright into tall grass.\r"
      "The grass goes up my nose and|\nFwafwafwafwafwa|\r"
      "Fwatchoo!",
      "" },
    { "Hiya! Are you maybe|\nA rookie Pok\xe9mon Trainer?\r"
      "Do you know what Pok\xe9mon Trainers do\nwhen they reach a new town?\r"
      "They first check what kind of\nPok\xe9mon Gym is in the town.",
      "" },

    // 120

    { "See? This is Petalburg City's\nPok\xe9mon Gym.\r"
      "See that symbol on the roof?\nThat marks this building as a Gym.\r"
      "Let me tell you something cool!\r"
      "The color represents the primary type\nof Pok\xe9mon the Gym Leader uses!\r"
      "For Petalburg, it is gray, since\nNorman uses Normal-type Pok\xe9mon.\r"
      "But he's probably too strong\nfor you anyways|",
      "" },
    { "Let's say you have six Pok\xe9mon.\nIf you catch another one|\r"
      "It is automatically sent to a Storage Box\nover a PC connection.",
      "" },
    { "My face is reflected in the water.\r"
      "It's a shining grin full of hope|\r"
      "Or it could be a look of somber silence\nstruggling with fear|\r"
      "What do you see reflected in your face?",
      "" },
    { "Where has our Wally gone?\r"
      "We have to leave for Verdanturf Town\nvery soon|",
      "" },
    { "Even if a Pok\xe9mon is weak now,\nit will grow stronger.\r"
      "The most important thing is love!\nLove for your Pok\xe9mon!",
      "" },

    { "Do you use Repel?\r"
      "It keeps Pok\xe9mon away, so it's\nuseful when you're in a hurry.",
      "" },
    { "Do you have any Antidotes with you?\r"
      "When a Pok\xe9mon is poisoned, it loses\nsome HP every turn in battle.\r"
      "Take some Antidotes with you to\ncure your poisoned Pok\xe9mon.",
      "" },
    { "The shelves brim with all sorts of\nPok\xe9mon merchandise.", "" },
    { "That PC-based Pok\xe9mon Storage System|\r"
      "Whoever made it must be some kind of a\nscientific wizard!",
      "" },
    { "When my Pok\xe9mon ate an\nOran Berry, it regained HP!", "" },

    // 130

    { "There are many types of Pok\xe9mon.\r"
      "All types have their strengths and\nweaknesses against other types.\r"
      "Depending on the types of Pok\xe9mon,\na battle could be easy or hard.",
      "" },
    { "I battled Norman once, but, whew,\nhe was way too strong.\r"
      "How would I put it?\r"
      "I just got the feeling that he\nlives for Pok\xe9mon.",
      "" },
    { "Norman became our town's new\nGym Leader.\r"
      "I think he called his family over\nfrom somewhere far away.",
      "" },
    { "There's a set of Pok\xe9mon picture books.", "" },
    { "It's filled with all sorts of books.", "" },

    { "Traveling is wonderful!\r"
      "When I was young, I roamed the seas\nand the mountains!",
      "" },
    { "Sigh|\r"
      "I wish I could go on an adventure\nwith some Pok\xe9mon|\r"
      "Crawl through some damp grass|\nClimb rocky, rugged mountains|\r"
      "Cross the raging seas|\nWander about in dark caves|\r"
      "And, sometimes, even get a little\nhomesick|\r"
      "It must be fabulous to travel!",
      "" },
    { "Petalburg City Pok\xe9mon Gym", "Bl\xfctenburg City Pok\xe9mon-Arena" },
    { "This door appears to be locked right now.", "Diese \xfcr ist verschlossen." },
    { "Dad: Hm?\r"
      "Well, if it isn't [PLAYER]!\nSo you're all finished moving in?\r"
      "I'm surprised that you managed to\nget here by yourself.\r"
      "Oh, I see.\nYou're with your Pok\xe9mon.\r"
      "Hm| Then I guess you're going to become\na Pok\xe9mon Trainer like me, [PLAYER].\r"
      "That's great news!\nI'll be looking forward to it!",
      "" },

    // 140

    { "Um| I|\nI'd like to get a Pok\xe9mon, please|", "" },
    { "Dad: Hm? You're| Uh| Oh, right.\nYou're Wally, right?", "" },
    { "Wally: I'm going to go stay with my\nrelatives in Verdanturf Town.\r"
      "I thought I would be lonely by myself,\nso I wanted to take a Pok\xe9mon along.\r"
      "But I've never caught a Pok\xe9mon before.\nI don't know how|",
      "" },
    { "Dad: Hm. I see.", "" },
    { "Dad: [PLAYER], you heard that, right?\r"
      "Go with Wally and make sure that he safely\ncatches a Pok\xe9mon.",
      "" },

    { "Wally, here, I'll loan you my Pok\xe9mon.", "" },
    { "Wally received a Zigzagoon!", "" },
    { "Wally: Oh wow!\nA Pok\xe9mon!\r"
      "Dad: Hm. I'll give you a Pok\xe9 Ball, too.\nGo for it!",
      "" },
    { "Wally received a Pok\xe9 Ball!", "" },
    { "Wally: Oh, wow!\nThank you!", "" },

    // 150

    { "[PLAYER]| Would you really come with me?", "" },
    { "Wally: [PLAYER]|\nPok\xe9mon hide in tall grass like this, right?\r"
      "Please watch me and see if I can\ncatch one properly.\r"
      "|Whoa!",
      "" },
    { "Wally: I did it| It's my|\nMy Pok\xe9mon!", "" },
    { "[PLAYER], thank you!\nLet's go back to the Gym!", "" },
    { "Dad: So, did it work out?", "" },

    { "Wally: Thank you, yes, it did.\nHere's your Pok\xe9mon back.\r"
      "[PLAYER], thank you for coming along with me.\r"
      "You two are why I was able to catch\nmy Pok\xe9mon.\r"
      "I promise I'll take really good care of it.\r"
      "Oh! My mom's waiting for me,\nso I have to go!\r"
      "Bye, [PLAYER]!",
      "" },
    { "Dad: Now|\r"
      "[PLAYER], if you want to become a\nstrong Trainer, here's my advice.\r"
      "Head for Rustboro City beyond this town.\r"
      "There, you should challenge\nthe Gym Leader, Roxanne.\r"
      "After her, go on to other Pok\xe9mon Gyms\nand defeat their Leaders.\r"
      "Collect Badges from them,\nunderstood?\r"
      "Of course, I'm a Gym Leader, too.\nWe'll battle one day, [PLAYER].\r"
      "But that's only after you become stronger.",
      "" },
    { "Dad: Hm? Aren't you going to the\nPok\xe9mon Gym in Rustboro City?\r"
      "There's no challenge for me to\nbattle a greenhorn Pok\xe9mon Trainer.\r"
      "[PLAYER], I want you to challenge me\nwhen you become a lot stronger.\r"
      "I'll battle you, [PLAYER], when you\ncan show me four Gym Badges, okay?",
      "" },
    { "Excuse me!\r"
      "Let me guess, from the way you're\ndressed, are you a Pok\xe9mon Trainer?",
      "" },
    { "| | | | |\r"
      "Well, maybe not.\nYour clothes aren't all that dirty.\r"
      "You're either a rookie Pok\xe9mon Trainer,\nor maybe you're just an ordinary kid.",
      "" },

    // 160

    { "I'm roaming the land in search of\ntalented Pok\xe9mon Trainers.\r"
      "I'm sorry to have taken your time.",
      "" },
    { "I wonder how our Wally is doing?", "" },
    { "Wally was really happy when he told\nus that he caught a Pok\xe9mon.\r"
      "It's been ages since I've seen him\nsmile like that." },
    { "You're|\nAh, you must be [PLAYER], right?\r"
      "Thank you for playing with Wally a\nlittle while ago.\r"
      "He's been frail and sickly ever\nsince he was a baby.\r"
      "We've sent him to stay with my relatives\nin Verdanturf Town for a while.\r"
      "The air is a lot cleaner there\nthan it is here.\r"
      "What's that? Where's Wally?\nHe's already left, our Wally.\r"
      "I wonder where he could have gotten by now?",
      "" },
    { "Wally: Thank you, yes, it did.\nHere's your Pok\xe9mon back.", "" },

    { "Dad: Is that a Ralts|?\r"
      "I've seen many Ralts already, but this\nis the first blue one I've met.\r"
      "Wally, you should treasure your new friend!",
      "" },
    { "Wally: I promise I'll take really good care of it.\r"
      "Oh! My mom's waiting for me,\nso I have to go!\r"
      "Bye, [PLAYER]!",
      "" },
    { "Route 104\nPetalburg City \x08", "Route 104\nBl\xfctenburg City \x08" },
    { "If you're going to throw a Pok\xe9 Ball,\nweaken the wild Pok\xe9mon first.\r"
      "It will be even easier to catch if it;s been\npoisoned, burned, or lulled to sleep.",
      "" },
    { "Oh, no, I'm not a Pok\xe9mon Trainer.\r"
      "But that's right, if Pok\xe9mon Trainers lock\neyes, it's a challenge to battle.\r"
      "If you don't want to battle, stay out\nof their sight.",
      "" },

    // 170

    { "Mr. Briney's Cottage", "" },
    { "The sea, huh?\r"
      "I wonder what it's like at the bottom\nof the sea?",
      "" },
    { "You're a thief if you try to steal\nsomeone else's Pok\xe9mon.\r"
      "You should throw Pok\xe9 Balls\nonly at wild Pok\xe9mon.",
      "" },
    { "--- Trainer Tips ---\r"
      "Any Pok\xe9mon that appears even once\nin a battle is awarded EXP Points.\r"
      "To raise a weak Pok\xe9mon, put it at the\ntop-left of the team list.\r"
      "As soon as a battle starts, switch it out.\nIt will earn EXP Points without taking damage.",
      "" },
    { "Sometimes, there ar things on the ground\neven if you can't see them.\r"
      "That's why I always check where I'm walking",
      "" },

    { "Hmmm|\nNot a single one to be found|", "" },
    { "Hello, have you seen any Pok\xe9mon\ncalled Shroomish around here?\r"
      "I really love that Pok\xe9mon.",
      "" },
    { "I wanted to ambush you, but you had to dawdle\nin Petalburg Woods forever, didn't you?\r"
      "I got sick of waiting, so here I am!",
      "" },
    { "You! Devon Researcher!\r"
      "Hand over those papers!",
      "" },
    { "Devon Researcher: Aiyeeh!\r"
      "You're a Pok\xe9mon Trainer, aren't you?\nYou've got to help me, please!",
      "" },

    // 180

    { "Would you like to help the Devon Researcher?", "" },
    { "Hunh? What do you think you're doing?\nWhat, you're going to protect him?\r"
      "No one who crosses Team Aqua\ngets any mercy, not even a kid!\r"
      "Come on and battle me!",
      "" },
    { "Grunt: Grr| You've got some\nnerve meddling with Team Aqua!\r"
      "Come on and battle me again!\r"
      "I wish I could say that, but I'm out of Pok\xe9mon|\r"
      "And, hey, we of Team Aqua are also\nafter something in Rustboro.\r"
      "I'll let you go today!",
      "" },
    { "Whew|\nThat was awfully close!\r"
      "Thanks to you, he didn't rob me of\nthese important papers.\r"
      "I know, I'll give you this Exp. All as\nmy thanks!",
      "" },
    { "Didn't that Team Aqua thug say they were\nafter something in Rustboro, too?", "" },

    { "Uh-oh! It's a crisis!\nI can't be wasting time!", "" },
    { "Grunt: See,\nyou are just no match for me!\r"
      "And now, stop intervening!",
      "" },
    { "The Team Aqua grunt stole the important\npapers from the Devon Researcher.", "" },
    { "Grunt: | | |\r"
      "Eh, what is this?\r"
      "Are you kidding me?\r"
      "I wasted my day for these mediocre\ndrawings of| Shroomish?\r"
      "But, hey, we of Team Aqua are also\nafter something in Rustboro.\r"
      "I'll let you go today!",
      "" },
    { "Whew|\nThat was awfully close!\r"
      "I'm sorry that I got you involved in this mess|\r"
      "I know, I'll give you this Exp. All as\nmy apology!",
      "" },

    // 190

    { "Tch, trying to hide behind a kid|", "" },
    { "Yo, there!\nYour Pok\xe9mon doing okay?\r"
      "If your Pok\xe9mon are weak and you don't want\nto battle, you should stay out of tall grass.",
      "" },
    { "--- Trainer Tips ---\r"
      "In addition to Hit Points (HP), Pok\xe9mon have\nPower Points (PP) that are used to\n"
      "make moves during battle.\r"
      "If a Pok\xe9mon runs out of PP, it must be\ntaken to a Pok\xe9mon Center.",
      "" },
    { "I like filling my mouth with seeds,\nthen spitting them out fast!\r"
      "You can have this to try it out!\r"
      "Use it on a Pok\xe9mon, and it will learn\na move for firing seeds rapidly.",
      "" },
    { "A word of advice!\r"
      "A TM, Technical Machine, is good for\nunlimited uses.\r"
      "In contrast, a TR, Technical Record, is\ngood only for one-time use.",
      "" },

    { "Pretty Petal Flower Shop", "" },
    { "--- Trainer Tips ---\r"
      "In the Hoenn region, there are pairs\nof Trainers who challenge others\n"
      "for 2-on-2 Pok\xe9mon battles called\nDouble Battles.\r"
      "In a Double Battle, the Trainer must send out\ntwo Pok\xe9mon, the ones at the top of the list.\r"
      "Watch how Pok\xe9mon are lined up.",
      "" },
    { "If you see Berries growing in loamy\nsoil, feel free to take them.\r"
      "But make sure you plant a Berry in the\nsame spot. That's common courtesy.\r"
      "Here, I'll share this with you.",
      "" },
    { "The way you look, you must be a\nPok\xe9mon Trainer, no?\r"
      "Trainers often make Pok\xe9mon hold Berries.\r"
      "It's up to you whether to grow Berries\nor use them.",
      "" },
    { "Route 104\n\x06 Rustboro City", "Route 104\n\x06 Metarost City" },

    // 200

    { "Rustboro City\r"
      "[129]The city probing the integration of\nnature and science.\"",
      "Metarost City\n[131][129]" },
    { "If a Pok\xe9mon has many battles, it can\nsometimes change in the way it looks.", "" },
    { "A Pok\xe9mon changes shape?\nIf one did that, I would be shocked!", "" },
    { "Hello!\r"
      "The more attention you give to flowers,\nthe more beautifully they bloom.\r"
      "You'll like tending flowers. I'm sure of it.\nYou can have this.",
      "" },
    { "While Berry plants are growing,\nwater them with the Wailmer Pail.\r"
      "Oh, another thing.\r"
      "If you don't pick Berries for a while,\nthey'll drop off onto the ground.\r"
      "But they'll sprout again.\r"
      "Isn't that awesome?\nIt's like they have the will to live.",
      "" },

    { "Hello!\r"
      "This is the Pretty Petal flower shop.\nSpreading flowers all over the world!",
      "" },
    { "Your name is?\r"
      "[PLAYER]\nThat's a nice name.",
      "" },
    { "[PLAYER], would you like to\nlearn about Berries?", "" },
    { "Flowers bring so much happiness to people,\ndon't they?", "" },
    { "Berries grow on trees that thrive only\nin soft, loamy soil.\r"
      "If you take some Berries, be sure to plant\none in the loamy soil again.\r"
      "A planted Berry will soon sprout,\ngrow into a plant, flower beautifully,\n"
      "then grow Berries again.\r"
      "I want to see the whole wide world\nfilled with beautiful flowers.\r"
      "That's my dream.\r"
      "Please help me, [PLAYER]. Plant Berries\nand bring more flowers into the world.",
      "" },

    // 210

    { "I'm trying to be like my big sisters.\nI'm growing flowers, too!\r"
      "Here you go!\nIt's for you!",
      "" },
    { "You can plant a Berry and grow it big,\nor you can make a Pokemon hold it.\r"
      "But now they have a machine that mixes up\ndifferent Berries and makes candies\n"
      "for Pok\xe9mon.\r"
      "I want some candy, too.",
      "" },
    { "Wow, you have Pok\xe9mon with you, too.\r"
      "When I get bigger, I'm going to go\nplaces with Pok\xe9mon, too.",
      "" },
    { "Pok\xe9mon Trainers' School\r"
      "If I go to this school, will I be able\nto catch rare Pok\xe9mon easily?",
      "" },
    { "Did you know this?\r"
      "You can have a 2-on-2 battle even if\nyou're not with another Trainer.\r"
      "If you catch the eyes of two Trainers\nwhen you have two or more Pok\xe9mon,\n"
      "they'll both challenge you.\r"
      "Don't you think it'd be cool if you\ncould beat two Trainers by yourself?",
      "" },

    { "Pok\xe9mon Trainers' School\n[129]We'll teach you anything about Pok\xe9mon\"", "" },
    { "Cutter's House", "Haus des Zerschneiders" },
    { "I challenged the Gym Leader, but|\r"
      "It's not going to be easy winning with my\nFire-type Pok\xe9mon|\r"
      "Fire-type Pok\xe9mon don't match up\nwell against Rock-type Pok\xe9mon|",
      "" },
    { "Devon Corporation\n[129]For all your living needs, we make it all.\"", "" },
    { "Have you taken tho Pok\xe9mon Gym challenge?\r"
      "When you get that shiny Gym Badge\nin hand, I guess Trainers begin to\n"
      "realize what is required of them.",
      "" },

    // 220

    { "The Devon Corporation|\nWe all just shorten it to Devon.\r"
      "That company makes all sorts of\nconvenient products.",
      "" },
    { "Ah, the sea|\nHow I miss the sea|\r"
      "The crushing waves below|\nThe restless sun above|\r"
      "How I wish I could experience that\nonce again|",
      "" },
    { "Would you like to offer your\nWishing Charm to the old man?", "" },
    { "Haha, thank you, but I can't accept\nsomething so precious.\r"
      "Besides, it would be wasted on such\nan old man as myself|\r"
      "My days as a sailor are long,\nlong gone, kid|\r"
      "| | | | |\r"
      "But that reminds me, you look like\na fine Pok\xe9mon Trainer.\r"
      "Perhaps this Old Sea Map is of\ninterest to you|",
      "" },
    { "Back in the day, I always tried to\nfind the place marked on the map,\n"
      "but I never succeeded.\r"
      "But you look like someone who is\nblessed with more luck than me|",
      "" },

    { "[129]Timesaving tunnel nearing completing\"\r"
      "|Is what it says on the sign, but there's\nalso a big [129]X\" splashed across it|",
      "" },
    { "Route 115\n[477] Rustboro City", "Route 115\n[477] Metarost City" },
    { "Exploring a cave isn't like walking on a road.\r"
      "You never know when wild Pok\xe9mon will appear.\nIt's full of suspense.",
      "" },
    { "Route 116\n[475] Rustboro City", "Route 116\n[475] Metarost City" },
    { "--- Trainer Tips ---\r"
      "If you want to stop a Pok\xe9mon from evolving,\npress the B Button while it is trying to evolve.\r"
      "The startled Pok\xe9mon will stop.\r"
      "This is called an evolution cancel.",
      "" },

    // 230

    { "Hello? Didn't we meet before?\nI think back in Petalburg City.\r"
      "Let me introduce myself.\nMy name's Scott.\r"
      "I've been traveling everywhere in\nsearch of outstanding Trainers.\r"
      "More specifically, I'm looking for\nPok\xe9mon battle experts.\r"
      "So, what brings you to this School?\nAre you a Trainer, too?\r"
      "The first thing you should do is to\nhave a Pok\xe9mon learn the move Cut.\r"
      "If I remenber correctly, someone in\nthis town has Cut.",
      "" },
    { "Scott: Hmm|\r"
      "The talent levels of the students here are\nunknown. The potentiel's there.",
      "" },
    { "It's this student's notebook|\r"
      "Pok\xe9mon are to be caught using Pok\xe9 Balls.\r"
      "Up to six Pok\xe9mon can accompany\na Pok\xe9mon Trainer.\r"
      "A Trainer is someone who catches Pok\xe9mon,\nraises them, and battles with them.\r"
      "A Trainer's mission is to defeat the strong\nTrainers who await challengers\n"
      "in Pok\xe9mon Gyms.",
      "" },
    { "Students who don't study get a little taste\nof my Quick Claw.\r"
      "Whether or not you are a good student will be\nevident from the way you use this item.",
      "" },
    { "A Pok\xe9mon holding the Quick Claw will\noccasionally speed up and get to move\n"
      "before its opponent.\r"
      "There are many other items that are meant\nto be held by Pok\xe9mon.\r"
      "Just those alone will give you\nmany topics to study!",
      "" },

    { "I always wanted a Seedot, and\nI'm finally getting one!", "" },
    { "I'm trading Pok\xe9mon with my friend right now.", "" },
    { "You know how some Pok\xe9mon moves can\nconfuse a Pok\xe9mon?\r"
      "A confused Pok\xe9mon will sometimes\nattack itself without meaning to.\r"
      "But once it leaves battle, it will\nreturn to normal.",
      "" },
    { "Pok\xe9mon can hold items, but they\ndon't know what to do with man-made items\n"
      "like Potion and Antidote.",
      "" },
    { "A Pok\xe9mon holding a Berry will heal itself|\r"
      "There are many kinds of items that\nPok\xe9mon can hold|\r"
      "Boy, it sure is hard taking notes down|",
      "" },

    // 240

    { "That determined expression|\nThat limber way you move|\r"
      "And your well-trained Pok\xe9mon|\r"
      "You're obviously a skilled Pok\xe9mon Trainer!\r"
      "No, wait, don't say a word.\nI can tell just by looking at you.\r"
      "I'm sure that you can put this\nHidden Machine to good use.\r"
      "No need to be modest or shy.\nGo on, take it!",
      "" },
    { "That Hidden Machine, or HM for short, is Cut.\r"
      "An HM move is one that can be used\nby Pok\xe9mon outside of battle.\r"
      "Any Pok\xe9mon that's learned Cut can\nchop down thin trees if the Trainer\n"
      "has earned the Stone Badge.",
      "" },
    { "When they were expanding the city of\nRustboro, my dad helped out.\r"
      "He made his Pok\xe9mon use Cut to clear\nthe land of trees.",
      "" },
    { "Oh, Who might you be?\nYou're a new face around these parts.\r"
      "Have you just transferred into the\nPok\xe9mon Trainers' School?",
      "" },
    { "The man next door gave me an HM!\r"
      "I used it to teach my Pok\xe9mon how to\nCut down skinny trees.",
      "" },

    { "My Pok\xe9mon has a Naive nature, and my\nfriend's has a Jolly nature.\r"
      "It's fascinating how Pok\xe9mon\nhave personalities!",
      "" },
    { "The old sailor Mr. Briney lives in a\ncottage by the sea.\r"
      "He goes for walks in the tunnel\nevery so often.",
      "" },
    { "My Pok\xe9mon evolved.\nIt has a lot of HP now.\r"
      "I should buy Super Potions for it\ninstead of ordinary Potions." },
    { "I'm getting an Escape Rope just in case\nI get lost in a cave.\r"
      "I just need to use it to get back to\nthe entrance.",
      "" },
    { "I'm buying some Paralyze Heals and\nAntidotes.\r"
      "Just in case I run into Shroomish\nin Petalburg Woods.",
      "" },

    // 250

    { "I'm sorry, only authorized people\nare allowed to enter here.", "" },
    { "Hello and welcome to the\nDevon Corporation.\r"
      "We're proud produces of items and\nmedicine that enhance your life.",
      "" },
    { "Yo, how's it going?\r"
      "Listen, my friend!\nWould you like to become the Champion?\r"
      "I'm no Trainer, not me, but I can\nsure give you winning advice.\r"
      "That's settled, then! We'll aim for the\nPok\xe9mon Championship together!\r"
      "It's your job as a Trainer to collect\nGym Badges, am I right?\r"
      "But Gym Leaders aren't pushovers!\r"
      "And that's where I come in!\nI'm here to provide expert advice!\r"
      "Roxanne, the Gym Leader, is a user of\nRock-type Pok\xe9mon.\r"
      "The Rock type is very durable, but it can't\nstand Water-type and Grass-type moves.\r"
      "Come and see me afterwards, if you beat\nthe Gym Leader.\r"
      "Well, go for it!",
      "" },
    { "Whoa! What a breathtaking victory!\nMy cheering must've worked!\r"
      "Great!\nCheck your Trainer's Card.\r"
      "The Stone Badge you got should be\nproperly recorded on it.\r"
      "In other words|\r"
      "You've climbed the first step on\nthe stairs to the Championship!\r"
      "That's got to feel awesome!",
      "" },
    { "Hello, I am Roxanne, the Rustboro\nPok\xe9mon Gym Leader.\r"
      "I became a Gym Leader so that I might\napply what I learned at the Pok\xe9mon\n"
      "Trainers' School in battle.\r"
      "Would you kindly demonstrate how you battle,\nand with which Pok\xe9mon?",
      "" },

    { "The Pok\xe9mon League's rules state that\nTrainers are to be given this Badge\n"
      "if they defeat a Gym Leader.\r"
      "Please accept the official\nPok\xe9mon League Stone Badge.",
      "" },
    { "The Stone Badge enables your Pok\xe9mon to\nuse the HM move Cut outside of battle.\r"
      "Further, now that you have a Gym Badge,\nstronger wild Pok\xe9mon will approach you.\r"
      "This TM here may help you\nwhen battling them.",
      "" },
    { "That Technical Machine, TM48,\ncontains Rock Tomb.\r"
      "It not only inflicts damage by dropping\nrocks, it also lowers Speed.\r"
      "If you use a TM, it instantly teaches\nthe move to a Pok\xe9mon.\r"
      "Remember, you can use a TM as often as you\nlike, so try to experiment a little bit.",
      "" },
    { "Since you are so strong, you should\nchallenge other Gym Leaders.\r"
      "By battling many Trainers, you should\nlearn many things.",
      "" },
    { "Rustboro City Pok\xe9mon Gym", "Metarost City Pok\xe9mon-Arena" },

    // 260

    { "[RIVAL]: Oh hey, [PLAYER]!\nYou're challanging the Pok\xe9mon Gym?\r"
      "Then let me see how strong you have\nbecome since we last battled!",
      "" },
    { "[RIVAL]: That was nice!\nYou sure became strong fast, [PLAYER].\r"
      "By the way, [PLAYER], how's you Pok\xe9"
      "Dex?\nHave you filled in any pages yet?\r"
      "Mine rules.\r"
      "Anyways, I gotta keep going.\nSo, see ya, [PLAYER]!",
      "" },
    { "Tunneler's Rest House", "" },
    { "Rusturf Tunnel\n[129]Linking Rustboro and Verdanturf\"\r"
      "[129]The tunnel project has been canceled.\"",
      "" },
    { "Nnn| Roar!\r"
      "I want to dig that tunnel!",
      "" },

    { "Get out!\nOut of the way!", "" },
    { "Wait! Pleeeaaase!\r"
      "Don't take my Goods!",
      "" },
    { "Oh, it's you!\r"
      "You're that fantastic Trainer who\nhelped me in Petalburg Woods!\r"
      "Help me! I was robbed by Team Aqua!\nI have to get the Devon Goods back!\r"
      "If I don't|\nI'm going to be in serious trouble.",
      "" },
    { "That shady character, I think he took\noff towards the tunnel over there.", "" },
    { "Nnn| Roar!\nWhat's going on?\r"
      "I was digging the tunnel without\nany tools when some goon ordered me out!\r"
      "That tunnel's filled with Pok\xe9mon\nthat react badly to loud noises.\r"
      "They could cause an uproar.\r"
      "That's why we stopped using heavy\nequipment for tunneling|\r"
      "I'm worried that the goon will do something\nstupid and startle the Pok\xe9mon into\n"
      "an uproar.",
      "" },

    // 270

    { "--- Trainer Tips ---\r"
      "Your Bag has several Pockets.\r"
      "Items you obtain are automatically\nplaced in the appropriate Pockets.\r"
      "No Trainer can afford to be\nwithout a Bag of their own.",
      "" },
    { "Oh, neat!\nThat's the Badge from Rustboro Gym!\r"
      "You must be a Trainer.\nYou should try using this item.",
      "" },
    { "It's a Miracle Seed.\r"
      "If a Pok\xe9mon holds that item, its Grass-type\nmoves become stronger.\r"
      "Besides Miracle Seed, I think there are other\nconvenient items for Pok\xe9mon to hold.",
      "" },
    { "Dad: I see| So, you've beaten\nthe Gym Leader in Rustboro City.\r"
      "But there are many more Trainers\nwho are stronger than you, [PLAYER].\r"
      "Across the sea in Dewford Town is a\nGym Leader named Brawly.\r"
      "You should challenge him.",
      "" },
    { "Better get going|", "" },

};

const char* const STRINGS[ MAX_STRINGS ][ LANGUAGES ] = {
    { "No data.", "Keine Daten." },
    { "Faraway Place", "Entfernter Ort" },
    { "Unknown", "Unbekannt" },
    { "%sUse %s?", "%sM\xf6"
                   "chtest du %s nutzen?" },
    { "The repel wore off.", "Der Schutz ist aufgebraucht." },

    { "Just an old Pok\xe9 Ball|", "Doch nur ein alter Pok\xe9"
                                   "ball|" },
    { "You fished a Pok\xe9mon!", "Du hast ein Pok\xe9mon geangelt!" },
    { "Map border.\nReturn, or you\nwill get lost!",
      "Ende der Kartendaten.\nKehr um, sonst\nverirrst du dich!" },
    { "\nSomething's on the hook!", "\nDa ist etwas am Haken!" },
    { "It disappeared|", "Es ist entkommen|" },

    // 10

    { "%s used %s!", "%s setzt %s ein!" },
    { "Items", "Items" },
    { "Medicine", "Medizin" },
    { "TM/HM", "TM/VM" },
    { "Berries", "Beeren" },

    { "Key Items", "Basis-Items" },
    { "Firmness: %s", "G\xfcte: %s" },
    { "Hard", "Hart" },
    { "Soft", "Weich" },
    { "Super hard", "Steinhart" },

    // 20

    { "Super soft", "Sehr weich" },
    { "Very hard", "Sehr hart" },
    { "Normal", "Normal" },
    { "Size:%4.1fcm", "Gr\xf6\xdf"
                      "e:%4.1fcm" },
    { "Spicy", "Scharf" },

    { "Dry", "Trocken" },
    { "Sweet", "S\xfc\xdf" },
    { "Bitter", "Bitter" },
    { "Sour", "Sauer" },
    { "Type", "Typ" },

    // 30

    { "Categ.", "Kateg." },
    { "PP", "AP" },
    { "Strength", "St\xe4rke" },
    { "Accuracy", "Genauigkeit" },
    { "Egg", "Ei" },

    { "Already learned", "Bereits erlernt" },
    { "Possible", "Erlernbar" },
    { "Not possible", "Nicht erlernbar" },
    { "Level %3d\n%3d/%3d HP", "Level %3d\n%3d/%3d KP" },
    { "Level %3d\nFainted", "Level %3d\nBesiegt" },

    // 40

    { "Possible", "M\xf6glich" },
    { "Not possible", "Nicht m\xf6glich" },
    { "No item", "Kein Item" },
    { "No items", "Keine Items" },
    { "Give", "Geben" },

    { "Apply", "Anwenden" },
    { "Register", "Registrieren" },
    { "Use", "Einsetzen" },
    { "Toss All", "Alle Wegwerfen" },
    { "Select a move!", "Welche Attacke?" },

    // 50

    { "Used %s.", "%s eingesetzt." },
    { "What?\n%s is evolving!", "Nanu?\n%s entwickelt sich!" },
    { "Congratulations! Your %s\nevolved into %s!", "Gl\xfc"
                                                    "ckwunsch! Dein %s\nwurde zu einem %s!" },
    { "It would be wasted|", "Es w\xfcrde keine Wirkung haben|" },
    { "%s already has an item.\nSwap the items?",
      "%s tr\xe4gt bereits ein Item.\nSollen die Items getauscht werden?" },

    { "You can't choose this item.", "Dieses Item kann nicht ausgew\xe4hlt werden." },
    { "Choose %s?", "%s ausw\xe4hlen?" },
    { "Do what with %s?", "Was tun mit %s?" },
    { "You can't use that right now.", "Das kann jetzt nicht eingesetzt werden." },
    { "Pok\xe9mon team", "Pok\xe9mon-Team" },

    // 60

    { "Cache", "Zwischenablage" },
    { "All boxes", "Alle Boxen" },
    { "New name for box %s", "Name f\xfcr Box %s" },
    { "Repel activated.", "Schutz eingesetzt." },
    { "Super Repel activated.", "Superschutz eingesetzt." },

    { "Max Repel activated.", "Top-Schutz eingesetzt." },
    { "Exp. All deactivated.", "EP-Teiler ausgeschaltet." },
    { "Exp. All activated.", "EP-Teiler eingeschaltet." },
    { "Coins: %lu.", "M\xfcnzen: %lu." },
    { "BP: %lu.", "Kampfpunkte: %lu." },

    // 70

    { "TOUCH TO START", "BER\xdc"
                        "HRE, UM ZU STARTEN" },
    { "Continue", "Weiter" },
    { "New Game", "Neues Spiel" },
    { "Special Episodes", "Spezialepisoden" },
    { "Import Save", "Spiel-Import" },

    { "Continue your adventure", "Setze dein Abenteuer fort" },
    { "Start a new adventure", "Starte ein neues Abenteuer" },
    { "Learn your companion's stories", "Erfahre mehr \xfc"
                                        "ber deine Begleiter" },
    { "Revive myths of old", "Setze alte Abenteuer fort" },
    { "This slot contains a saved game.\nOverwrite it?",
      "Es existiert bereits ein Spielstand.\nSoll dieser \xfc"
      "berschrieben werden?" },

    // 80

    { "Yes", "Ja" },
    { "No", "Nein" },
    { "(empty)", "(leer)" },
    { "Transfer", "Transfer" },
    { "Special", "Spezial" },

    { "The language is set to English.\nChoose a different language?",
      "Die aktuelle Sprache ist Deutsch.\nSprache wechseln?" },
    { "You put the %s\ninto the [%hu] %s bag.", "%s in der [%hu] %s-Tasche verstaut." },
    { "Stored %s %s", "%s %s in der" },
    { "Stored %s", "%s in der" },
    { "Stored %3d %s", "%3d %s in der" },

    // 90

    { "The scent vanished|", "Der Duft verstrich wirkungslos|" },
    { "Pok\xe9Nav", "Pok\xe9Nav" },
    { " Would you like to save your progress?", " M\xf6"
                                                "chtest du deinen Fortschritt sichern?" },
    { "Saving a lot of data| Don't cut the power.\n%s",
      "Speichere eine Menge Daten| Nicht abschalten.\n%s" },
    { "Saving complete!", "Speichern erfolgreich!" },

    { "An error occured.\nProgress not saved.", "Es trat ein Fehler auf\nSpiel nicht gesichert." },
    { "No %s available.\nRemove the icon?",
      "Kein Exemplar des Items\n%s vorhanden.\nIcon entfernen?" },
    { "Recently used items\nwill appear here.", "Hier erscheinen zuletzt\neingesetzte Items." },
    { "You can register an item to the Y button.", "Du kannst ein Item auf Y registrieren." },
    { "%s used %s!", "%s setzt %s ein!" },

    // 100

    { "You can't use this\nmove right now.", "Diese Attacke kann jetzt\nnicht eingesetzt werden." },
    { "Took %s\nfrom %s.", "%s von\n%s verstaut." },
    { "%s already\nknows %s!", "%s beherrscht\n%s bereits!" },
    { "%s learned\n%s!", "%s erlernt\n%s!" },
    { "%s already knows 4 moves. Forget a move?",
      "%s beherrscht bereits 4 Attacken. Soll eine verlernt werden?" },

    { "Select a move!", "Welche Attacke?" },
    { "%s can't\nforget %s!", "%s kann\n%s nicht vergessen!" },
    { "%s can't\nlearn %s!", "%s kann\n%s nicht erlernen!" },
    { "%hhu badges", "%hhu Orden" },
    { "Pick a language", "W\xe4hle eine Sprache" },

    // 110

    { "Choose an episode", "W\xe4hle eine Speizalepisode" },
    { "Welcome to Special Episode 0:\nDeveloper's Heaven.",
      "Willkommen zur Spezialepisode 0:\nDeveloper's Heaven" },
    { "In this episode, you will gain\nthe mighty powers of the\ndev himself.",
      "In dieser Episode erh\xe4ltst du\ndie F\xe4higkeiten des Entwicklers." },
    { "Do whatever you want.", "Mach damit, was immer du m\xf6"
                               "chtest." },
    { "Do you want to import your\nsave from the GBA game?",
      "M\xf6"
      "chtest du deinen Spielstand\nvon dem GBA-Modul auf dem DS\nfortsetzen?" },

    { "Importing a save file will\ncopy all Pok\xe9mon and items\nfrom the GBA game.",
      "Beim Importieren werden alle\nPok\xe9mon und Items vom\nGBA-Modul kopiert." },
    { "Further, you will start at\nthe same position as you\nleft off in the GBA game.",
      "Du wirst das Spiel an der\nselben Stelle wie auf dem\nGBA-Modul fortsetzen." },
    { "No data will be written to\nthe GBA Game Pak.",
      "Es werden keine Daten auf\ndas GBA-Modul geschrieben." },
    { "However, proceed at your\nown risk.",
      "Trotzdem geschieht das\nImportieren auf eigene\nGefahr." },
    { "No compatible cartrige found.\nAborting.",
      "Kein kompatibles GBA-Modul\ngefunden.\nBreche ab." },

    // 120

    { "Proceed?", "Fortfahren?" },
    { "Importing data|", "Lade Spielstand|" },
    { "An error occured.\nAborting.", "Ein Fehler ist aufgetreten.\nKehre zum Hauptmen\xfc zur\xfc"
                                      "ck." },
    { "Completed.", "Abgeschlossen." },
    { "Entries", "Eintr\xe4ge" },

    { "All", "Alle" },
    { "HP", "KP" },
    { "Attack", "Angriff" },
    { "Defense", "Vert." },
    { "Sp. Atk", "Sp. Ang." },

    // 130

    { "Sp. Def", "Sp. Ver." },
    { "Speed", "Init." },
    { "f. %s", "aus %s" },
    { "female", "weiblich" },
    { "male", "m\xe4nnlich" },

    { "(shining)", "(schillernd)" },
    { "Fainted", "Besiegt" },
    { "Statistics", "Statuswerte" },
    { "Lv.%3i", "Lv.%3i" },
    { "%s tries to learn %s.", "%s versucht %s zu lernen." },

    // 140

    { "The terrain didn't change.", "Das Terrain blieb unver\xe4ndert." },
    { "Do you want to give a nick name to %s?", "M\xf6"
                                                "chtest du dem %s einen Spitznamen geben?" },
    { "Choose a nick name!", "W\xe4hle einen Spitznamen!" },
    { "You are challenged by %s %s!", "Eine Herausforderung von %s %s!" },
    { "Frzn", "Eingfr" },

    { "Prlz", "Prlyse" },
    { "Burn", "Verbrng" },
    { "Slp", "Schlaf" },
    { "Psn", "Gift" },
    { "Switch", "Aussenden" },

    // 150

    { "In Battle", "Im Kampf" },
    { "Send In", "In den Kampf" },
    { "To be sent", "Schon ausgew\xe4hlt" },
    { "Eggs can't hold items!", "Ein Ei kann kein Item tragen." },
    { "battle!", "nicht k\xe4mpfen!" },

    { "Status", "Bericht" },
    { "Moves", "Attacken" },
    { "ATK", "ANG" },
    { "DEF", "VER" },
    { "SAT", "SAN" },

    // 160

    { "SDF", "SVE" },
    { "SPD", "INI" },
    { "What will %s do?", "Was soll %s tun?" },
    { "You got away safely.", "Du bist entkommen." },
    { "You couldn't escape|", "Flucht gescheitert|" },

    { "Choose a target!", "Welches Pok\xe9mon angreifen?" },
    { "Choose a Pok\xe9mon.", "Welches Pok\xe9mon?" },
    { "%s gained %lu EXP.", "%s gewinnt %lu E.-Punkte." },
    { "%s advanced to level %d.", "%s erreicht Level %d." },
    { "The %s enabled\n%s to go first!", "%.0s%s agiert dank\neiner %s zuerst!" },

    // 170

    { "The wild %s fled!", "%s (wild) floh." },
    { "But nothing happened|", "Nichts geschah|" },
    { "It got wasted|", "Es hatte keine Wirkung|" },
    { "Used %s.", "%s eingesetzt." },
    { "%s's data was\nregistred to the Pok\xe9"
      "Dex.",
      "Die Daten von %s\nwurden im Pok\xe9"
      "Dex gespeichert." },

    { "%s was sent to\nthe Pok\xe9mon Storage System.",
      "%s wurde an das\nPok\xe9mon-Lagerungssystem\ngeschickt." },
    { "Box [129]%s\" is full.", "Box [131]%s[129] ist voll." },
    { "%s was deposited to Box [129]%s\".", "%s wurde in Box [131]%s[129] abgelegt." },
    { "There is no space left for additional Pok\xe9mon.",
      "Du hast keinen Platz f\xfcr weitere Pok\xe9mon." },
    { "%s was released.", "%s wurde wieder\nfreigelassen." },

    // 180

    { "FIGHT", "Kampf" },
    { "BAG", "Beutel" },
    { "POK\xe9MON", "Pok\xe9mon" },
    { "RUN", "Flucht" },
    { "Pwr", "Stk" },

    { "Meteor Falls", "Meteorf\xe4lle" },
    { "\xd", "\xe" }, // HP icon
    { "Hardy", "Robust" },
    { "Lonely", "Einsam" },
    { "Brave", "Mutig" },

    // 190

    { "Adamant", "Hart" },
    { "Naughy", "Frech" },
    { "Bold", "K\xfc"
              "hn" },
    { "Docile", "Sanft" },
    { "Relaxed", "Locker" },

    { "Impish", "Pfiffig" },
    { "Lax", "Lasch" },
    { "Timid", "Scheu" },
    { "Hasty", "Hastig" },
    { "Serious", "Ernst" },

    // 200

    { "Jolly", "Froh" },
    { "Naive", "Naiv" },
    { "Modest", "M\xe4"
                "\xdf"
                "ig" },
    { "Mild", "Mild" },
    { "Quiet", "Ruhig" },

    { "Bashful", "Zaghaft" },
    { "Rash", "Hitzig" },
    { "Calm", "Still" },
    { "Gentle", "Zart" },
    { "Sassy", "Forsch" },

    // 210

    { "Careful", "Sacht" },
    { "Quirky", "Kauzig" },
    { "Loves to eat.", "Liebt es, zu essen." },
    { "Often dozes off.", "Nickt oft ein." },
    { "Nods off a lot.", "Schl\xe4"
                         "ft gerne." },

    { "Scatters things often.", "Macht oft Unordnung." },
    { "Likes to relax.", "Liebt es zu entspannen." },
    { "Proud of its power.", "Ist stolz auf seine St\xe4"
                             "rke." },
    { "Likes to thrash about.", "Pr\xfc"
                                "gelt sich gerne." },
    { "A little quick tempered.", "Besitzt Temperament." },

    // 220

    { "Likes to fight.", "Liebt es zu k\xe4"
                         "mpfen." },
    { "Quick tempered.", "Ist impulsiv." },
    { "Sturdy body.", "Hat einen robusten K\xf6"
                      "rper." },
    { "Capable of taking hits.", "Kann Treffer gut verkraften." },
    { "Highly persistent.", "Ist \xe4"
                            "u\xdf"
                            "erst ausdauernd." },

    { "Good endurance.", "Hat eine gute Ausdauer." },
    { "Good perseverance.", "Ist beharrlich." },
    { "Highly curious.", "Ist sehr neugierig." },
    { "Mischievous.", "Ist hinterh\xe4"
                      "ltig." },
    { "Thoroughly cunning.", "Ist \xe4"
                             "u\xdf"
                             "erst gerissen." },

    // 230

    { "Often lost in thought.", "Ist oft in Gedanken." },
    { "Very finicky.", "Ist sehr pedantisch." },
    { "Strong willed.", "Besitzt starken Willen." },
    { "Somewhat vain.", "Ist etwas eitel." },
    { "Strongly defiant.", "Ist sehr aufs\xe4"
                           "ssig." },

    { "Hates to lose.", "Hasst Niederlagen." },
    { "Somewhat stubborn.", "Ist dickk\xf6"
                            "pfig." },
    { "Likes to run.", "Liebt es, zu rennen." },
    { "Alert to sounds.", "Achtet auf Ger\xe4"
                          "usche." },
    { "Impetuous and silly.", "Ist ungest\xfc"
                              "m und einf\xe4"
                              "ltig." },

    // 240

    { "Somewhat of a clown.", "Ist fast wie eine Clown." },
    { "Quick to flee.", "Fl\xfc"
                        "chtet schnell." },
    { "spicy", "scharf" },
    { "sour", "saur" },
    { "sweet", "s\xfc\xdf" },

    { "dry", "trocken" },
    { "bitter", "bitter" },
    { "all", "all" },
    { "HP", "HP" },
    { "Attack", "Angriff" },

    // 250

    { "Defense", "Verteidigung" },
    { "Sp. Attack", "Spez Angr." },
    { "Sp. Def.", "Sepz. Vert." },
    { "Speed", "Initiative" },
    { "Evasion", "Fluchtwert" },

    { "Accuracy", "Genauigkeit" },
    { "%s's %s did not change.", "%.0s%s von %s blieb unver\xe4ndert." },
    { "%s's %s rose.", "%.0s%s von %s stieg." },
    { "%s's %s rose sharply.", "%.0s%s von %s stieg stark." },
    { "%s's %s rose drastically.", "%.0s%s von %s stieg drastisch." },

    // 260

    { "%s's %s fell.", "%.0s%s von %s sank." },
    { "%s's %s harshly fell.", "%.0s%s von %s sank stark." },
    { "%s's %s severely fell.", "%.0s%s von %s sank drastisch." },
    { "%s %s sends out %s.", "%s %s schickt %s in den Kampf." },
    { "F I G H T", "K A M P F" },

    { "POK\xc9MON", "POK\xc9MON" },
    { "RUN", "FLUCHT" },
    { "BAG", "BEUTEL" },
    { "BACK", "ZUR\xdc"
              "CK" },
    { "%s sets up a trap.", "%s legt eine Falle." },

    // 270

    { "%s is tightening its focus.", "%s versch\xe4rft seinen Fokus." },
    { "%s's beak glows.", "Der Schnabel von %s gl\xfcht." },
    { "%s returned to you.", "%s kam zu dir zur\xfc"
                             "ck" },
    { "That is enough %s.", "Das reicht, %s." },
    { "%s returned to %s.", "%s kam zu %s zur\xfc"
                            "ck." },

    { "%s withdrew %s.", "%s rief %s zur\xfc"
                         "ck." },
    { "%s recharges.", "%s l\xe4"
                       "dt sich wieder auf." },
    { "%s became confused.", "%s ist vor Ersch\xf6pfung verwirrt." },
    { "%s ate its %s. Its confusion vanished.", "%s a\xdf die %s. Es ist nicht mehr verwirrt." },
    { "%s ate its %s.", "%s a\xdf die %s." },

    // 280

    { "%s evaded the attack.", "%s wich aus." },
    { "%s passes its %s. The %s found its purpose.",
      "%s gibt seine %s weiter. Die %s fand ihre Bestimmung." },
    { "The %s protects %s.", "Die %s sch\xfctzt %s." },
    { "%s obtained the %s.", "%s erhielt das Item %s." },
    { "It has no effect on %s|", "Es hat keine Wirkung auf %s|" },

    { "It is super effective on %s.", "Es ist sehr effektiv gegen %s." },
    { "It is not very effective on %s.", "Es ist nicht sehr effektiv gegen %s." },
    { "%s is hit by the recoil.", "Der R\xfc"
                                  "cksto\xdf schadet %s." },
    { "%s restores some HP.", "%s heilt ein paar KP." },
    { "%s fainted.", "%s wurde besiegt." },

    // 290

    { "It missed %s.", "%s wich aus." },
    { "A critical hit!", "Ein Volltreffer!" },
    { "%s prepares a move.", "%s bereitet einen Angriff vor." },
    { "%s is confused.", "%s ist verwirrt." },
    { "%s snapped out of its confusion.", "%s ist nicht mehr verwirrt." },

    { "It hurt itself in its confusion.", "Es hat sich in seiner Verwirrung selbst verletzt." },
    { "%s flinches.", "%s schreckt zur\xfc"
                      "ck." },
    { "%s is frozen solid.", "%s ist gefroren." },
    { "%s was thawed.", "%s ist aufgetaut." },
    { "%s is asleep.", "%s schl\xe4"
                       "ft." },

    // 300

    { "%s woke up.", "%s ist aufgewacht." },
    { "%s is paralyzed. It can't move.", "%s ist paralysiert. Es kann nicht angreifen." },
    { "%s is immobilized by its love.", "%s ist starr vor Liebe." },
    { "The weather didn't change.", "Das Wetter bleibt unver\xe4ndert." },
    { "It failed.", "Es schlug fehl." },

    { "%s became fully charged due to its Power Herb!", "Dank Energiekraut ist %s sofort bereit!" },
    { "%s looses some HP.", "%s verliert ein paar KP." },
    { "%s made a wish upon its %s.", "%s w\xfcnscht sich St\xe4rke von seinem %s." },
    { "%s's wish for power was granted!", "Der Wunsch von %s wurde erh\xf6hrt!" },
    { "the wild %s", "%s (wild)" },

    // 310

    { "the foe's %s", "%s (Gegner)" },
    { "The wild %s", "%s (wild)" },
    { "The foe's %s", "%s (Gegner)" },
    { "This tree looks like it can be cut down. ", "Ein kleiner Baum. " },
    { "This rock appears to be breakable. ", "Ein kleiner Felsen. " },

    { "A huge whirlpool. ", "Ein riesiger Strudel. " },
    { "The water is a deep blue| ", "Das Wasser ist tiefblau| " },
    { "The sea is deep here. ", "Das Wasser ist sehr tief. " },
    { "It's a big boulder. ", "Ein gro\xdf"
                              "er Felsen. " },
    { "The wall is very rocky|\n", "Eine steile Felswand|\n" },

    // 320

    { "It's a large Waterfall. ", "Ein gro\xdf"
                                  "er Wasserfall. " },
    { "Something moved! ", "Da hat sich etwas bewegt! " },
    { "Light is shining from the surface. ", "Licht scheint von der Oberfl\xe4"
                                             "che. " },
    { "Select", "Ausw\xe4hlen" },
    { "Status", "Status" },

    { "Give Item", "Item geben" },
    { "Take Item", "Item nehmen" },
    { "Use Item", "Item nutzen" },
    { "Swap", "Austauschen" },
    { "Pok\xe9"
      "Dex",
      "Pok\xe9"
      "Dex" },

    // 330

    { "Cancel", "Zur\xfc"
                "ck" },
    { "Deselect", "Abw\xe4hlen" },
    { "Select %hhu Pok\xe9mon!", "W\xe4hle %hhu Pok\xe9mon!" },
    { "Select a Pok\xe9mon!", "W\xe4hle ein Pok\xe9mon!" },
    { "Gave %s\nto %s.", "%s an\n%s gegeben." },

    { "Choose these Pok\xe9mon?", "Diese Pok\xe9mon w\xe4hlen?" },
    { "Choose this Pok\xe9mon?", "Dieses Pok\xe9mon w\xe4hlen?" },
    { "Dex No.", "Dex Nr." },
    { "Name", "Name" },
    { "OT", "OT" },

    // 340

    { "ID No.", "Id Nr." },
    { "Exp.", "E.-Pkt." },
    { "Next", "N\xe4"
              "chst." },
    { "Level", "Level" },
    { "Pok\xe9mon Info", "Pok\xe9mon-Info" },

    { "It looks like this Egg", "Was da wohl schl\xfcpfen" },
    { "will take a long time", "wird? Es dauert wohl" },
    { "to hatch.", "noch lange." },
    { "It appears to move", "Hat es sich gerade" },
    { "occasionally. It may", "bewegt? Da tut sich" },

    // 350

    { "be close to hatching.", "wohl bald was." },
    { "Sounds can be heard", "Jetzt macht es schon" },
    { "coming from inside!", "Ger\xe4usche! Bald ist" },
    { "It will hatch soon!", "es wohl soweit!" },
    { "A mysterious Pok\xe9mon", "Ein r\xe4tselhaftes" },

    { "Egg received", "Pok\xe9mon-Ei erhalten" },
    { "from", "vom" },
    { "in/at", "in/bei" },
    { "from", "von" },
    { "A highly mysterious", "Ein \xe4u\xdf"
                             "erst seltsames" },

    // 360

    { "Pok\xe9mon Egg that", "Pok\xe9mon-Ei, das" },
    { "somehow reached you", "irgendwie zu dir fand" },
    { "Item", "Item" },
    { "Ability", "F\xe4higkeit" },
    { "", "Wesen: " },

    { " nature.", "" },
    { "Egg received.", "Ei erhalten." },
    { "Had a fateful encunter.", "Schicksalhafte Begegnung." },
    { "Egg hatched.", "Ei geschl\xfcpft." },
    { "Egg apparently hatched.", "Ei offenbar geschl\xfcpft." },

    // 370

    { "Met at Lv. %hu.", "Erhalten mit Lv. %hu." },
    { "Apparently met at Lv. %hu.", "Offenbar erhalten mit Lv. %hu." },
    { "Had a fateful enc. at Lv. %hu.", "Schicksalhafte Begeg. mit Lv. %hu." },
    { "App. had a fatef. enc. at Lv. %hu.", "Off. schicks. Begeg. mit Lv. %hu." },
    { "Likes ", "Mag " },

    { "%s ", "%se " },
    { "Pok\xe9"
      "blocks.",
      "Pok\xe9riegel." },
    { "%2hhu/%2hhu", "%2hhu/%2hhu" }, // PP
    { "Nature", "Wesen" },
    { "Move Pok\xe9mon", "Pok\xe9mon bewegen" },

    // 380

    { "Show Party", "Team-PKMN" },
    { "Hide Party", "Box-PKMN" },
    { "Move", "Bewegen" },
    { "Summary", "Details" },
    { "Release", "Freilassen" },

    { "Give Item", "Item geben" },
    { "Take Item", "Item nehm." },
    { "Cancel", "Abbrechen" },
    { "Huh? The evolution\nwas interrupted|", "Huh? Die Entwicklung\nwurde abgebrochen|" },
    { "%s hatched from the Egg!", "%s schl\xfcpfte aus dem Ei!" },

    // 390
    { "Power: %hhu", "St\xe4rke: %hhu" },
    { "Accuracy: %hhu", "Genauigkeit: %hhu" },
    { "(never misses)", "(trifft immer)" },
    { "%s's\n%s%.0s", "%.0s%s\nvon %s" },
    { "You encountered a wild %s!", "Ein wildes %s erscheint!" },

    { "Go %s!", "Los %s!" },
    { "%s detects %s.", "%s erkennt %s." },                 // Forewarn
    { "%s shudders.", "%s erschaudert." },                  // Anticipation
    { "%s identifies %s.", "%s erkennt %s." },              // Frisk
    { "%s identifies %s and %s.", "%s erkennt %s und%s." }, // Frisk

    // 400

    { "Hit %hhu times.", "%hhu Mal getroffen." },
    { "Details", "Details" },
    { "Deregister", "Registrieren" },
    { "%s did not learn %s.", "%s hat %s nicht erlernt." },
    { "Mystery Gift", "Geheimgeschehen" },

    { "T R A I N E R'S   C A R D", "T R A I N E R P A S S" },
    { "Name", "Name" },
    { "Play time", "Spielzeit" },
    { "Money", "Geld" },
    { "Pok\xe9"
      "Dex",
      "Pok\xe9"
      "Dex" },

    // 410

    { "Adventure started on %s", "Abenteuer gest. am %s" },
    { "Last save created in/at %s\non %s, %02hhu:%02hhu",
      "Zuletzt gesp. in/bei %s\nam %s um %02hhu:%02hhu." },
    { "No save data in this slot.", "Kein Spielstand in diesem Slot." },
    { "Pok\xe9mon", "Pok\xe9mon" },
    { "Pok\xe9"
      "Dex",
      "Pok\xe9"
      "Dex" },

    { "Bag", "Beutel" },
    { "Trainer ID", "Trainerpass" },
    { "Save", "Speichern" },
    { "Options", "Optionen" },
    { "Text Speed", "Textgeschw." },

    // 420

    { "Language", "Sprache" },
    { "BGM", "Musik" },
    { "SFX", "Soundeffekte" },
    { "Difficulty", "Schwierigkeit" },
    { "Wallpaper", "Hintergrund" },

    { "Slow", "Langs." },
    { "Medium", "Mittel" },
    { "Fast", "Schnell" },
    { "On", "An" },
    { "Off", "Aus" },

    // 430
    { "Wynaut", "Isso" },
    { "Skitty", "Eneco" },
    { "Absol", "Absol" },
    { "Sound disabled", "Musik deaktiviert" },
    { "Hoenn Pok\xe9mon League Badges", "Hoenn Pok\xe9mon-Liga Orden" },

    { "Battle Frontier Symbols", "Kampfzonensymbole" },
    { "%s obtained\nthe %s!", "%s erhielt\n%s!" },
    { "Choose the %s Pok\xe9mon %s?", "Das %s-Pok\xe9mon %s w\xe4hlen?" },
    { "Prof. Birch is in trouble!\nRelease a Pok\xe9mon and rescue him!",
      "Prof. Birk ist in Schwierigkeiten!\nSchnapp' dir ein Pok\xe9mon und hilf ihm!" },
    { "BACKSPACE", "ENTFERNEN" },

    // 440

    { "ENTER", "ENTER" },
    { "Hello..?\nCan you hear me?", "Hallo..?\nKannst du mich h\xf6ren?" },
    { "Nice! Your Pok\xe9Nav seems to be working just fine.",
      "Gut! Dein Pok\xe9Nav funktioniert schonmal." },
    { "My name is Birch, but everyone calls me Pok\xe9mon Professor.",
      "Mein Name ist Birk, aber jeder nennt mich Pok\xe9mon Professor." },
    { "But first of all, welcome to the world of Pok\xe9mon!",
      "Aber zun\xe4"
      "chst, willkommen in der Welt der Pok\xe9mon!" },

    { "This is what we call a [129]Pok\xe9mon.\"",
      "Dies ist ein sogenanntes [131]Pok\xe9mon[129]." },
    { "This world is widely inhabited by creatures known as Pok\xe9mon.",
      "Auf dieser Welt leben Wesen, die als Pok\xe9mon bekannt sind." },
    { "We humans live alongside Pok\xe9mon, at times as friendly playmates,",
      "Wir Menschen leben Seite an Seite mit ihnen, manchmal als Freunde," },
    { "at times as cooperative workmates.", "manchmal auch als Arbeitskollegen." },
    { "And sometimes, we band together and battle others like us.",
      "Und manchmal schlie\xdf"
      "en wir uns zusammen und tragen K\xe4mpfe aus." },

    // 450

    { "But despite our closeness, there is much to learn about Pok\xe9mon.",
      "Und dennoch gibt es noch viel \xfc"
      "ber die Pok\xe9mon zu lernen." },
    { "In fact, there are many, many secrets surrounding Pok\xe9mon.",
      "Es gibt noch viele, viele Geheimnisse um die Pok\xe9mon." },
    { "To unravel Pok\xe9mon mysteries, I've been undertaking research.",
      "Um ihre Mysterien aufzudecken, widme ich der Forschung." },
    { "That's what I do.", "Ja, genau das tue ich." },
    { "Ah sorry, I got carried away|", "Ah, ich schweife ab|" },

    { "I am calling because I need some information from you.",
      "Ich rufe an, weil ich noch ein paar Informationen von dir ben\xfctige." },
    { "For now, just complete your profile in your Pok\xe9Nav.",
      "Kannst du bitte dein Pok\xe9Nav-Profil ausf\xfcllen?" },
    { "Choose a picture!", "W\xe4hle ein Bild!" },
    { "Your name?", "Dein Name?" },
    { "Are you fine with your profile?", "Ist dieses Profil so in Ordnung?" },

    // 460

    { "Brendan", "Brix" },
    { "May", "Maike" },
    { "Alright ", "Also gut " },
    { ", are you ready?", ", bist du bereit?" },
    { "Your very own adventure is about to unfold.",
      "Dein ganz eigenes Abenteuer wird nun beginnen." },

    { "Take courage, and leap into the world of Pok\xe9mon,",
      "Hab nur Mut, und st\xfcrze dich in die Welt der Pok\xe9mon," },
    { "where dreams, adventure, and friendships await!",
      "wo Abenteuer, Tr\xe4ume und Freundschaften warten." },
    { "Well, I'll be expecting you later.\nCome see me in my Pok\xe9mon Lab.",
      "Ah, komm sp\xe4ter in mein Pok\xe9mon-\nLabor, ich erwarte dich dort." },
    { "Buy", "Kaufen" },
    { "Sell", "Verkaufen" },

    // 470

    { "Welcome!\nHow may I help you today?", "Willkommen!\nWie kann ich helfen?" },
    { "Money: $%lu", "Geld: $%lu" },
    { "BP: %lu", "GP: %lu" },
    { "Coins: %lu", "M\xfcnzen: %lu" },
    { "In Bag: %lu", "Beutel: %lu" },

    { "%s, sure!\nHow many do you need?", "%s, gerne!\nWie viele Exemplare ben\xf6tigst du?" },
    { "%s, and you want %ld? This will be $%lu then.", "%s und du m\xf6"
                                                       "chtest %ld? Das macht dann $%lu." },
    { "%s, and you want %ld? This will be %lu BP then.", "%s und du m\xf6"
                                                         "chtest %ld? Das macht dann %lu GP." },
    { "%s, and you want %ld? This will be %lu coins then.",
      "%s und du m\xf6"
      "chtest %ld? Das macht dann %lu M\xfcnzen." },
    { "You don't have enough money.", "Du hast nicht genug Geld." },

    // 480

    { "You don't have enough BP.", "Du hast nicht genug GP." },
    { "You don't have enough coins.", "Du hast nicht genug M\xfcnzen." },
    { "I cannot buy this item.", "Ich kann dieses Item nicht kaufen." },
    { "Sell All", "Alle Verkaufen" },
    { "I can pay you $%lu. Is this fine?", "Ich kann $%lu zahlen. Ist das ok?" },

    { "Do you really want to toss this item?", "Willst du dieses Item wirklich wegwerfen?" },
    { "Gotcha! %s was caught!", "Toll! %s wurde gefangen!" },
    { "Oh, no! It broke free!", "Mist! Es hat sich befreit!" },
    { "Aww! It appeared to be caught!", "Oh. Fast h\xe4tte es geklappt!" },
    { "Aargh! Almost had it!", "Mist! Das war knapp!" },

    // 490

    { "Shoot! It was so close, too!", "Verflixt! Es war doch fast gefangen." },
    { "The weather became normal again.", "Das Wetter wurde wieder normal." },
    { "It started to rain.", "Es begann zu regnen." },
    { "It started to hail.", "Es begann zu hageln." },
    { "The air became foggy.", "Dichter Nebel zog auf." },

    { "A sandstorm formed.", "Ein Sandsturm zog auf." },
    { "The sunlight turned harsh.", "Das Sonnenlicht wurde st\xe4rker." },
    { "A heavy rain began to fall.", "Es begann enornm zu regnen." },
    { "The sunlight turned extremely harsh.", "Das Sonnenlicht wurde enorm kr\xe4rker." },
    { "Mysterious strong winds are protecting Flying-type Pok\xe9mon.",
      "Starke Winde sch\xfctzen Flug-Pok\xe9mon." },

    // 500
    { "Rain continues to fall.", "Es regnet weiter." },
    { "Hail continues to fall.", "Es hagelt." },
    { "The air is foggy.", "Dichter Nebel." },
    { "The sandstorm rages.", "Der Sandsturm w\xfctet" },
    { "The sunlight is harsh.", "Das Sonnenlicht ist stark." },

    { "Heavy rain continues to fall.", "Enormer Regen." },
    { "The extremely harsh sunlight continues.", "Extremes Sonnenlicht." },
    { "Strong winds continue to blow.", "Starke Winde." },
    { "The battlefield became normal.", "Das Feld wurde normal." },
    { "The battlefield became weird.", "Das Feld wurde komisch." },

    // 510

    { "An electric current runs across the battlefield.",
      "Eine elektrische Str\xf6mung flie\xdft durch das Feld." },
    { "Mist swrils around the battlefield.", "Das Feld wurde in Nebel geh\xfcllt." },
    { "Grass grew to cover the battlefield.", "Gras wuchs \xfc"
                                              "ber das Feld." },
    { "A deluge of ions showers the battlefield.",
      "Ein elektrisch geladener Niederschlag regnet auf das Feld herab." },
    { "It created a strange room in which items no longer work.",
      "Es entsteht ein Raum, in dem keine Items mehr funktionieren." },

    { "It created a bizarre area in which the Defense and Sp. Defense stats are swapped.",
      "Es entsteht ein Raum, in dem Vert. und Sp.Vert. vertauscht sind." },
    { "It twisted the dimensions.", "Es hat die Dimensionen verdreht." },
    { "The gravity intensifies.", "Die Schwerkraft wurde st\xe4rker." },
    { "Fire's power was weakened.", "Die St\xe4rke des Feuers wurde geschw\xe4"
                                    "cht." },
    { "Electricity's power was weakened.", "Die St\xe4rke der Elektrizit\xe4t wurde geschw\xe4"
                                           "cht." },

    // 520

    { "All Pok\xe9mon become locked to the field for the next turn.",
      "Alle Pok\xe9mon sind f\xfcr eine Runde an das Feld gebunden." },
    { "The ion shower disappeared.", "Der elektrisch geladene Niederschlag verschwand." },
    { "The Magic Room disappeared.", "Der Magieraum verschwand." },

    { "The Wonder Room disappeared.", "Der Wunderraum verschwand." },
    { "The twisted dimensions returned to normal.", "Die Dimensionen wurden wieder normal." },

    { "The gravity became normal again.", "Die Schwerkraft wurde wieder normal." },
    { "Fire's power returned to normal", "Die St\xe4rke des Feuers wurde wieder normal" },
    { "Electricity's power returned to normal.",
      "Die St\xe4rke der Elektrizit\xe4t wurde wieder normal." },
    { "The Fairy Lock stopped.", "Feenschloss l\xe4sst nach." },
    { "%s is poisoned.", "%s ist vergiftet." },

    // 530

    { "%s is badly poisoned.", "%s ist schwer vergiftet." },
    { "%s is burned.", "%s ist verbrannt." },
    { "%s restores some HP by listening to the soothing sound of its Shell Bell.",
      "%s lauscht dem beruhigenden Klang seines Seegesangs und heilt ein paar KP." },
    { "%s is harmed by its curse.", "Der Fluch schadet %s." },
    { "%s restores some HP using its roots.", "Die Wurzeln von %s heilen ein paar KP." },

    { "%s's Aqua Ring restores some HP.", "Der Wasserring von %s heilt ein paar KP." },
    { "%s is harmed by its nightmare.", "Der Alptraum schadet %s." },
    { "%s activated its %s.", "%s nutzt sein Item %s." },
    { "%s's Air Balloon popped.", "Der Luftballon von %s platzte." },
    { "%s drank its %s.", "%s trank sein %s." },

    // 540

    { "%s dives.", "%s taucht ab." },
    { "%s dug a hole.", "%s gr\xe4"
                        "bt sich ein." },
    { "%s flies up high.", "%s flog hoch." },
    { "%s jumps up high.", "%s sprang hoch." },
    { "%s <SKYDROP>.", "%s <SKYDROP>" },

    { "%s disappeared.", "%s verschwand." },
    { "%s continues and crashes.", "%s macht weiter und bricht zusammen." },
    { "It's a one-hit KO!", "Ein K.O.-Treffer!" },
    { "%s lost its focus and cannot attack.",
      "%s hat seinen Fokus verloren und kann nicht angreifen." },
    { "%s's trap failed.", "Die Falle von %s blieb wirkungslos." },

    // 550

    { "Pok\xe9mon Trainer", "Pok\xe9mon-Trainer" },
    { "%s %s used %s.", "%s %s setzt %s ein." },
    { "You earned $%lu.", "Du gewinnst $%lu." },
    { "You payed $%lu to the winner.", "Du zahlst dem Gewinner $%lu." },
    { "Ash: $%lu grams", "Asche: $%lu Gramm" },

    { "You don't have enough ash.", "Du hast nicht genug Asche." },
    { "%s? I can make one for %lu grams ash.", "%s? Alles klar, das macht dann %lu Gramm Asche." },
    { "Collected ash: %lu grams.", "Gesammelte Asche: %lu Gramm." },
    { "A big boulder that became moveable thanks to your Pok\xe9mon's Strength.",
      "Ein gro\xdf"
      "er Felsen, der durch die St\xe4rke deiner Pok\xe9mon bewegbar wurde." },
    { "You booted up the PC.", "Du hast den PC angeschaltet." },

    // 560

    { "It's a map of the Hoenn region.", "Eine Karte der Hoenn-Region." },
    { "You scurried to a Pok\xe9mon Center,\nprotecting your exhausted\n"
      "and fainted Pok\xe9mon\nfrom further harm|",
      "Du rennst zu einem Pok\xe9mon-Center\num deine ersch\xf6pten und\n"
      "besiegten Pok\xe9mon vor\nweiterem Schaden zu sch\xfctzen|" },
    { "You scurried back home,\nprotecting your exhausted\n"
      "and fainted Pok\xe9mon\nfrom further harm|",
      "Du rennst zurück nach Hause\num deine ersch\xf6pten\n"
      "und besiegten Pok\xe9mon vor\nweiterem Schaden zu sch\xfctzen|" },
    { "Obtained %hu of %s.", "%hu %s erhalten." },
    { "Obtained %s.", "%s erhalten." },

    { "One %s was planted here.", "Eine %s wurde hier gepflanzt." },
    { "The %s that was planted here has sprouted.",
      "Die %s, die hier geflanzt wurde ist ausgetrieben." },
    { "The %s plant is growing bigger.", "Die %s w\xe4"
                                         "chst." },
    { "The %s plant is in full bloom.", "Die %s bl\xfcht." },
    { "The %s plant carries %hhu berries.", "Die %s tr\xe4gt %hhu Beeren." },

    // 570

    { "Would you like to harvest the berries?", "M\xf6"
                                                "chtest du die Beeren ernten?" },
    { "The soil is very soft.\nWould you like to plant a berry?",
      "Der Boden ist sehr weich\nM\xf6"
      "chtest du eine Beere einpflanzen?" },
    { "Planted a %s.", "%s eingepflanzt." },
    { "You watered the berry plant.", "Du hast die Beerenpflanze gegossen." },
    { "Would you like to water the berries?", "M\xf6"
                                              "chtest du die Beeren gie?\xdf"
                                              "en?" },

    { "No. ", "Nr. " },

};
