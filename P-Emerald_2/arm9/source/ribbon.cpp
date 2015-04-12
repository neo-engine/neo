#include "ribbon.h"
#include <initializer_list>

std::vector<u8> ribbon::getRibbons( const pokemon& p_pokemon ) {
    std::vector<u8> res;
    u8 curr = 0;
    for( u8 j : { p_pokemon.m_boxdata.m_ribbons2[ 0 ],
         p_pokemon.m_boxdata.m_ribbons2[ 1 ],
         p_pokemon.m_boxdata.m_ribbons2[ 2 ],
         p_pokemon.m_boxdata.m_ribbons2[ 3 ],

         p_pokemon.m_boxdata.m_ribbons0[ 0 ],
         p_pokemon.m_boxdata.m_ribbons0[ 1 ],
         p_pokemon.m_boxdata.m_ribbons0[ 2 ],
         p_pokemon.m_boxdata.m_ribbons0[ 3 ],

         p_pokemon.m_boxdata.m_ribbons1[ 0 ],
         p_pokemon.m_boxdata.m_ribbons1[ 1 ],
         p_pokemon.m_boxdata.m_ribbons1[ 2 ],
         p_pokemon.m_boxdata.m_ribbons1[ 3 ] } )
         for( u8 i = 0; i < 8; ++i, ++curr )
             if( j & ( 1 << i ) )
                 res.push_back( curr );
    return res;
}

ribbon RibbonList[ MAX_RIBBONS ] = {
    //0x3c (Contest Ribbons, Hoenn)
    { "Band der Coolness", "Pokémon- Wettbewerb\n Sieger in der Normal-Klasse!" },
    { "Band der Coolness Super", "Pokémon- Wettbewerb\n Sieger in der Super-Klasse!" },
    { "Band der Coolness Hyper", "Pokémon- Wettbewerb\n Sieger in der Hyper-Klasse!" },
    { "Band der Coolness Master", "Pokémon- Wettbewerb\n Sieger in der Master-Klasse!" },

    { "Band der Schönheit", "Pokémon- Wettbewerb\n Sieger in der Normal-Klasse!" },
    { "Band der Schönheit Super", "Pokémon- Wettbewerb\n Sieger in der Super-Klasse!" },
    { "Band der Schönheit Hyper", "Pokémon- Wettbewerb\n Sieger in der Hyper-Klasse!" },
    { "Band d. Schönh. Master", "Pokémon- Wettbewerb\n Sieger in der Master-Klasse!" },

    //0x3d
    { "Band der Anmut", "Pokémon- Wettbewerb\n Sieger in der Normal-Klasse!" },
    { "Band der Anmut Super", "Pokémon- Wettbewerb\n Sieger in der Super-Klasse!" },
    { "Band der Anmut Hyper", "Pokémon- Wettbewerb\n Sieger in der Hyper-Klasse!" },
    { "Band der Anmut Master", "Pokémon- Wettbewerb\n Sieger in der Master-Klasse!" },

    { "Band der Klugheit", "Pokémon- Wettbewerb\n Sieger in der Normal-Klasse!" },
    { "Band der Klugheit Super", "Pokémon- Wettbewerb\n Sieger in der Super-Klasse!" },
    { "Band der Klugheit Hyper", "Pokémon- Wettbewerb\n Sieger in der Hyper-Klasse!" },
    { "Band der Klugh. Master", "Pokémon- Wettbewerb\n Sieger in der Master-Klasse!" },

    //0x3e
    { "Band der Stärke", "Pokémon- Wettbewerb\n Sieger in der Normal-Klasse!" },
    { "Band der Stärke Super", "Pokémon- Wettbewerb\n Sieger in der Super-Klasse!" },
    { "Band der Stärke Hyper", "Pokémon- Wettbewerb\n Sieger in der Hyper-Klasse!" },
    { "Band der Stärke Master", "Pokémon- Wettbewerb\n Sieger in der Master-Klasse!" },

    { "Band des Champs von Hoenn", "Band für das Erreichen des Titels Champ und den Eintritt in die Ruhmeshalle." },
    { "Band des Gewinners", "Band für Siege in der Lv. 50- Kategorie im Duellturm von Hoenn." },
    { "Band des Sieges", "Band für Siege in der Lv. 100- Kategorie im Duellturm von Hoenn." },
    { "Band des Künstlers", "Belohnung für die Wahl zum Super-Model in Hoenn." },

    //0x3f
    { "Fleiß-Band", "Band für extrem fleißige Arbeiter." },
    { "Band des Meeres", "" },
    { "Band der Landmassen", "" },
    { "Band der Lüfte", "" },

    { "Band des Landes", "Pokémon Liga\nBand des Champs" },
    { "Band der Nation", "Band für den erfolgreichen Abschluss aller Schwierigkeits- grade." },
    { "Band der Erde", "100. Sieg in Folge Gedenk-Band" },
    { "Band der Welt", "Pokémon Liga\nBand des Champs" },

    // 0x24
    { "Band des Champs von Sinnoh", "Band für den Sieg über den Sinnoh Champ und den Eintritt in die Ruhmeshalle." },
    { "Band der Fähigkeit", "Ein Band, verliehen zum Sieg über den Kampfkoloss des Duellturms." },
    { "Großes Band der Fähigkeit", "Ein Band, verliehen zum Sieg über den Kampfkoloss des Duellturms." },
    { "Doppel-Band der Fähigkeit", "Ein Band für die Meisterung des Doppels im Sinnoh Duellturm." },

    { "Multi-Band der Fähigkeit", "Ein Band für die Meisterung der Multi- Herausforderung im Sinnoh Duellturm." },
    { "Paar-Band der Fähigkeit", "Ein Band für die Meisterung der Link-Multi- Herausforderung im Sinnoh Duellturm." },
    { "Welt-Band der Fähigkeit", "Ein Band für die Meisterung der Wi-Fi- Herausforderung im Sinnoh Duellturm." },
    { "Band der Wachsamkeit", "Ein Band zur Erinnerung an ein stärkendes, Lebensenergie gebendes Erlebnis." },

    //0x25
    { "Band des Schocks", "Ein Band zur Erinnerung an ein Ereignis, das das Leben aufregender machte." },
    { "Band des Niederschlags", "Ein Band zur Erinnerung an die Traurigkeit, die dem Leben Würze gab." },
    { "Band der Sorglosigkeit", "Ein Band zur Erinnerung an einen Fehler, der zu wichtigen Entscheidungen führte." },
    { "Band der Entspannung", "Ein Band zur Erinnerung an ein erfrischendes Ereignis, das dem Leben Glanz gab." },

    { "Band des Schlafens", "Ein Band zur Erinnerung an einen tiefen Schlaf, der den Fluss des Lebens beruhigte." },
    { "Band des Lächelns", "Ein Band zur Erinnerung daran, dass ein Lächeln das Leben bereichert." },
    { "Hinreißendes Band", "Ein außergewönlich schönes und extravagantes Band." },
    { "Königliches Band", "Ein unglaublich königliches Band, das eine Aura der Erhabenheit ausstrahlt." },

    //0x26
    { "Hinreißendes Königliches Band", "Ein wunderschönes und königliches Band, das einfach sagenhaft ist." },
    { "Fußabdruck- Band", "Ein Band für ein Pokémon, das einen besonders guten Fußabdruck hinterlässt." },
    { "Band des Rekords", "Ein Band für das Aufstellen eines unglaublichen Rekords." },
    { "Band der Geschichte", "Ein Band für das Aufstellen eines historischen Rekords." },

    { "Band der Legende", "Ein Band für das Aufstellen eines legendären Rekords." },
    { "Rotes Band", "" }, //!
    { "Grünes Band", "" }, //!
    { "Blaues Band", "" }, //!

    //0x27
    { "Festival-Band", "Pokémon-Festival Teilnehmer-Band" },
    { "Jahrmarkt-Band", "" },
    { "Klassisches Band", "" },
    { "Premierband", "" },

    { "Sonderband", "Besonderes Band für einen besonderen Tag." },
    { "Kampfmeister- band", "Band des Siegers eines Kampfturniers." },
    { "Regionalmeister- band", "Band des Siegers eines Kampfturniers." },
    { "Nationalmeister- band", "Band des Siegers eines Kampfturniers." },

    //0x60 (Contest Ribbons, Sinnoh)
    { "Band der Coolness", "Super-Pokémon- Wettbewerb\n Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der Coolness Mega", "Super-Pokémon- Wettbewerb\n Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der Coolness Ultra", "Super-Pokémon- Wettbewerb\n Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band der Coolness Master", "Super-Pokémon- Wettbewerb\n Sieger in der Master-Klasse in Sinnoh!" },

    { "Band der Schönheit", "Super-Pokémon- Wettbewerb\n Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der Schönheit Mega", "Super-Pokémon- Wettbewerb\n Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der Schönheit Ultra", "Super-Pokémon- Wettbewerb\n Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band d. Schönh. Master", "Super-Pokémon- Wettbewerb\n Sieger in der Master-Klasse in Sinnoh!" },

    //0x61
    { "Band der Anmut", "Super-Pokémon- Wettbewerb\n Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der Anmut Mega", "Super-Pokémon- Wettbewerb\n Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der Anmut Ultra", "Super-Pokémon- Wettbewerb\n Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band der Anmut Master", "Super-Pokémon- Wettbewerb\n Sieger in der Master-Klasse in Sinnoh!" },

    { "Band der Klugheit", "Super-Pokémon- Wettbewerb\n Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der Klugheit Mega", "Super-Pokémon- Wettbewerb\n Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der Klugheit Ultra", "Super-Pokémon- Wettbewerb\n Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band der Klugh. Master", "Super-Pokémon- Wettbewerb\n Sieger in der Master-Klasse in Sinnoh!" },

    //0x62
    { "Band der Stärke", "Super-Pokémon- Wettbewerb\n Sieger in der Normal-Klasse in Sinnoh!" },
    { "Band der Stärke Mega", "Super-Pokémon- Wettbewerb\n Sieger in der Mega-Klasse in Sinnoh!" },
    { "Band der Stärke Ultra", "Super-Pokémon- Wettbewerb\n Sieger in der Ultra-Klasse in Sinnoh!" },
    { "Band der Stärke Master", "Super-Pokémon- Wettbewerb\n Sieger in der Master-Klasse in Sinnoh!" },

    { "Weltmeister- band", "Band des Siegers eines Kampfturniers." },
    { "Geburtstags- band", "Ein Band um einen Geburtstag zu feiern." },
    { "", "" },
    { "", "" },

    //0x63
    { "", "" },
    { "", "" },
    { "", "" },
    { "", "" },

    { "", "" },
    { "", "" },
    { "", "" },
    { "", "" },
};