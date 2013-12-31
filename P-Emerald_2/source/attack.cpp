#include "attack.h"

#define C (char)
#define A (attack::AffectsTypes)
#define F (attack::Flags)
#define H (attack::HitTypes)
attack AttackList[560] = {
    //attack(_Name,     _Effect,_Base_Power,_type,    _Accuracy, _PP,_E.Accuracy,_Aff._whom,_Priority,Flags,_HitType)
    attack("----",          C 0,    C 0,    UNBEKANNT,  C 0,     C 0,    C 0,        A 0,    C 0,    F 0,    H 0),
    attack("Pfund",         C 0,    C 40,   NORMAL,     C 0,     C 35,   C 0,        A 0,    C 1,    F 35,   H 0),
    attack("Karateschlag",  C 0,    C 50,   KAMPF,      C 90,    C 25,   C 0,        A 0,    C 1,    F 35,   H 0),
    attack("Duplexhieb",    C 5,    C 15,   NORMAL,     C 85,    C 25,   C 0,        A 0,    C 1,    F 35,   H 0),
    attack("Kometenhieb",   C 5,    C 18,   NORMAL,     C 85,    C 25,   C 0,        A 0,    C 1,    F 35,   H 0),
    attack("Megahieb",      C 0,    C 80,   NORMAL,     C 85,    C 20,   C 0,        A 0,    C 1,    F 35,   H 0),
    attack("Zahltag",       C 6,    C 40,   NORMAL,     C 0,     C 20,   C 0,        A 0,    C 1,    F 50,   H 0),
    attack("Feuerschlag",   C 7,    C 80,   FEUER,      C 0,     C 15,   C 30,       A 0,    C 1,    F 35,   H 0),
    attack("Eishieb",       C 8,    C 80,   EIS,        C 0,     C 15,   C 30,       A 0,    C 1,    F 35,   H 0),
    attack("Donnerschlag",  C 9,    C 80,   ELEKTRO,    C 0,     C 15,   C 30,       A 0,    C 1,    F 35,   H 0),
    attack("Kratzer",       C 0,    C 40,   NORMAL,     C 0,     C 35,   C 0,        A 0,    C 1,    F 35,   H 0),
    attack("Klammer",       C 0,    C 55,   NORMAL,     C 0,     C 30,   C 0,        A 0,    C 1,    F 35,   H 0),
    attack("Guillotine",    C 10,   C 0,    NORMAL,     C 0,     C 5,    C 30,       A 0,    C 1,    F 35,   H 0),
    attack("Klingensturm",  C 11,   C 80,   NORMAL,     C 0,     C 10,   C 0,        A 0,    C 1,    F 34,   H 1),
    attack("Schwerttanz",   C 12,   C 0,    NORMAL,     C 0,     C 30,   C 0,        A 16,   C 1,    F 8,    H 2),
    cut(),
    attack("Windstoß",      C 0,    C 40,   FLUG,       C 0,     C 35,   C 0,        A 0,    C 1,    F 34,   H 1),
    attack("Flügelschlag",  C 0,    C 60,   FLUG,       C 0,     C 35,   C 0,        A 0,    C 1,    F 35,   H 0),
    attack("Wirbelwind",    C 13,   C 0,    FLUG,       C 0,     C 20,   C 0,        A 0,    C 1,    F 38,   H 2),
    fly(),
    attack("Klammergriff",  C 14,   C 15,   NORMAL,     C 75,    C 20,   C 0,        A 0,    C 1,    F 35,   H 0),    
    attack("Slam",          C 0,    C 80,   NORMAL,     C 75,    C 20,   C 0,        A 0,    C 1,    F 35,   H 0),    
    attack("Rankenhieb",    C 0,    C 35,   PFLANZE,    C 0,     C 15,   C 0,        A 0,    C 1,    F 35,   H 0),    

    rock_smash(),
    flash(),
    whirlpool(),
    surf()
    /*

023 | Stampfer | Normal | Physisch | 65 | 100 | 20 | Stärke 

024 | Doppelkick | Kampf | Physisch | 30 | 100 | 30 | Coolness 

025 | Megakick | Normal | Physisch | 120 | 75 | 5 | Coolness 

026 | Sprungkick | Kampf | Physisch | 85 | 95 | 25 | Coolness 

027 | Fegekick | Kampf | Physisch | 60 | 85 | 15 | Coolness 

028 | Sandwirbel | Boden | Status | 0 | 100 | 15 | Anmut 

029 | Kopfnuss | Normal | Physisch | 70 | 100 | 15 | Stärke 

030 | Hornattacke | Normal | Physisch | 65 | 100 | 25 | Coolness 

031 | Furienschlag | Normal | Physisch | 15 | 85 | 20 | Coolness 

032 | Hornbohrer | Normal | Physisch | K.O. | 30 | 5 | Coolness 

033 | Tackle | Normal | Physisch | 35 | 95 | 35 | Stärke 

034 | Bodyslam | Normal | Physisch | 85 | 100 | 15 | Stärke 

035 | Wickel | Normal | Physisch | 15 | 85 | 20 | Stärke 

036 | Bodycheck | Normal | Physisch | 90 | 85 | 20 | Stärke 

037 | Fuchtler | Normal | Physisch | 90 | 100 | 20 | Stärke 

038 | Risikotackle | Normal | Physisch | 120 | 100 | 15 | Stärke 

039 | Rutenschlag | Normal | Status | 0 | 100 | 30 | Anmut 

040 | Giftstachel | Gift | Physisch | 15 | 100 | 35 | Klugheit 

041 | Duonadel | Käfer | Physisch | 25 | 100 | 20 | Coolness 

042 | Nadelrakete | Käfer | Physisch | 14 | 85 | 20 | Coolness 

043 | Silberblick | Normal | Status | 0 | 100 | 30 | Coolness 

044 | Biss | Normal | Physisch | 60 | 100 | 25 | Stärke 

045 | Heuler | Normal | Status | 0 | 100 | 40 | Anmut 

046 | Brüller | Normal | Status | 0 | 100 | 20 | Coolness 

047 | Gesang | Normal | Status | 0 | 55 | 15 | Anmut 

048 | Superschall | Normal | Status | 0 | 55 | 20 | Klugheit 

049 | Ultraschall | Normal | Spezial | 20 (KP) | 90 | 20 | Coolness 

050 | Aussetzer | Normal | Status | 0 | 80 | 20 | Klugheit 

051 | Säure | Gift | Spezial | 40 | 100 | 30 | Klugheit 

052 | Glut | Feuer | Spezial | 40 | 100 | 25 | Schönheit 

053 | Flammenwurf | Feuer | Spezial | 95 | 100 | 15 | Schönheit 

054 | Weißnebel | Eis | Status | 0 | 100 | 30 | Schönheit 

055 | Aquaknarre | Wasser | Spezial | 40 | 100 | 25 | Anmut 

056 | Hydropumpe | Wasser | Spezial | 120 | 80 | 5 | Schönheit 

057 | Surfer | Wasser | Spezial | 95 | 100 | 15 | Schönheit 

058 | Eisstrahl | Eis | Spezial | 95 | 100 | 10 | Schönheit 

059 | Blizzard | Eis | Spezial | 120 | 70 | 5 | Schönheit 

060 | Psystrahl | Psycho | Spezial | 65 | 100 | 20 | Schönheit 

061 | Blubbstrahl | Wasser | Spezial | 65 | 100 | 20 | Schönheit 

062 | Aurorastrahl | Eis | Spezial | 65 | 100 | 20 | Schönheit 

063 | Hyperstrahl | Normal | Spezial | 150 | 90 | 5 | Coolness 

064 | Schnabel | Flug | Physisch | 35 | 100 | 35 | Coolness 

065 | Bohrschnabel | Flug | Physisch | 80 | 100 | 20 | Coolness 

066 | Überroller | Kampf | Physisch | 80 | 80 | 25 | Coolness 

067 | Fußkick | Kampf | Physisch | Variiert (Gewicht) | 100 | 20 | Stärke 

068 | Konter | Kampf | Physisch | Variiert | 100 | 20 | Stärke 

069 | Geowurf | Kampf | Physisch | Variiert (Level) | 100 | 20 | Stärke 

070 | Stärke | Normal | Physisch | 80 | 100 | 15 | Stärke 

071 | Absorber | Pflanze | Spezial | 20 | 100 | 25 | Klugheit 

072 | Megasauger | Pflanze | Spezial | 40 | 100 | 15 | Klugheit 

073 | Egelsamen | Pflanze | Status | 0 | 90 | 10 | Klugheit 

074 | Wachstum | Normal | Status | 0 | 100 | 40 | Schönheit 

075 | Rasierblatt | Pflanze | Physisch | 55 | 95 | 25 | Coolness 

076 | Solarstrahl | Pflanze | Spezial | 120 | 100 | 10 | Coolness 

077 | Giftpuder | Gift | Status | 0 | 75 | 35 | Klugheit 

078 | Stachelspore | Pflanze | Status | 0 | 75 | 30 | Klugheit 

079 | Schlafpuder | Pflanze | Status | 0 | 75 | 15 | Klugheit 

080 | Blättertanz | Pflanze | Spezial | 90 | 100 | 20 | Schönheit 

081 | Fadenschuss | Käfer | Status | 0 | 95 | 40 | Klugheit 

082 | Drachenwut | Drache | Spezial | 40 (KP) | 100 | 10 | Coolness 

083 | Feuerwirbel | Feuer | Spezial | 15 | 70 | 15 | Schönheit 

084 | Donnerschock | Elektro | Spezial | 40 | 100 | 30 | Coolness 

085 | Donnerblitz | Elektro | Spezial | 95 | 100 | 15 | Coolness 

086 | Donnerwelle | Elektro | Status | 0 | 100 | 20 | Coolness 

087 | Donner | Elektro | Spezial | 120 | 70 | 10 | Coolness 

088 | Steinwurf | Gestein | Physisch | 50 | 90 | 15 | Stärke 

089 | Erdbeben | Boden | Physisch | 100 | 100 | 10 | Stärke 

090 | Geofissur | Boden | Physisch | K.O. | 30 | 5 | Stärke 

091 | Schaufler | Boden | Physisch | 80 | 100 | 10 | Klugheit 

092 | Toxin | Gift | Status | 0 | 85 | 10 | Klugheit 

093 | Konfusion | Psycho | Spezial | 50 | 100 | 25 | Klugheit 

094 | Psychokinese | Psycho | Spezial | 90 | 100 | 10 | Klugheit 

095 | Hypnose | Psycho | Status | 0 | 70 | 20 | Klugheit 

096 | Meditation | Psycho | Status | 0 | 100 | 40 | Schönheit 

097 | Agilität | Psycho | Status | 0 | 100 | 30 | Coolness 

098 | Ruckzuckhieb | Normal | Physisch | 40 | 100 | 30 | Coolness 

099 | Raserei | Normal | Physisch | 20 | 100 | 20 | Coolness 

100 | Teleport | Psycho | Status | 0 | 100 | 20 | Coolness 

101 | Nachtnebel | Geist | Spezial | Variiert (Level) | 100 | 15 | Klugheit 

102 | Mimikry | Normal | Status | Variiert | 100 | 10 | Anmut 

103 | Kreideschrei | Normal | Status | 0 | 85 | 40 | Klugheit 

104 | Doppelteam | Normal | Status | 0 | 100 | 15 | Coolness 

105 | Genesung | Normal | Status | 0 | 100 | 10 | Klugheit 

106 | Härtner | Normal | Status | 0 | 100 | 30 | Stärke 

107 | Komprimator | Normal | Status | 0 | 100 | 20 | Anmut 

108 | Rauchwolke | Normal | Status | 0 | 100 | 20 | Klugheit 

109 | Konfustrahl | Geist | Status | 0 | 100 | 10 | Klugheit 

110 | Panzerschutz | Wasser | Status | 0 | 100 | 40 | Anmut 

111 | Einigler | Normal | Status | 0 | 100 | 40 | Anmut 

112 | Barriere | Psycho | Status | 0 | 100 | 30 | Coolness 

113 | Lichtschild | Psycho | Status | 0 | 100 | 30 | Schönheit 

114 | Dunkelnebel | Eis | Status | 0 | 100 | 30 | Schönheit 

115 | Reflektor | Psycho | Status | 0 | 100 | 20 | Klugheit 

116 | Energiefokus | Normal | Status | 0 | 100 | 30 | Coolness 

117 | Geduld | Normal | Physisch | Variiert | 100 | 10 | Stärke 

118 | Metronom | Normal | Status | 0 | 100 | 10 | Anmut 

119 | Spiegeltrick | Flug | Status | 0 | 100 | 20 | Klugheit 

120 | Finale | Normal | Physisch | 200 | 100 | 5 | Schönheit 

121 | Eierbombe | Normal | Physisch | 100 | 75 | 10 | Stärke 

122 | Schlecker | Geist | Physisch | 20 | 100 | 30 | Stärke 

123 | Smog | Gift | Spezial | 20 | 70 | 20 | Stärke 

124 | Schlammbad | Gift | Spezial | 65 | 100 | 20 | Stärke 

125 | Knochenkeule | Boden | Physisch | 65 | 85 | 20 | Stärke 

126 | Feuersturm | Feuer | Spezial | 120 | 85 | 5 | Schönheit 

127 | Kaskade | Wasser | Physisch | 80 | 100 | 15 | Stärke 

128 | Schnapper | Wasser | Physisch | 35 | 75 | 10 | Stärke 

129 | Sternschauer | Normal | Spezial | 60 | 100 | 20 | Coolness 

130 | Schädelwumme | Normal | Physisch | 100 | 100 | 15 | Stärke 

131 | Dornkanone | Normal | Physisch | 20 | 100 | 15 | Coolness 

132 | Umklammerung | Normal | Physisch | 10 | 100 | 35 | Stärke 

133 | Amnesie | Psycho | Status | 0 | 100 | 20 | Anmut 

134 | Psykraft | Psycho | Status | 0 | 80 | 15 | Klugheit 

135 | Weichei | Normal | Status | 0 | 100 | 10 | Schönheit 

136 | Turmkick | Kampf | Physisch | 100 | 90 | 20 | Coolness 

137 | Giftblick | Normal | Status | 0 | 75 | 30 | Stärke 

138 | Traumfresser | Psycho | Spezial | 100 | 100 | 15 | Klugheit 

139 | Giftwolke | Gift | Status | 0 | 55 | 40 | Klugheit 

140 | Stakkato | Normal | Physisch | 15 | 85 | 20 | Stärke 

141 | Blutsauger | Käfer | Physisch | 20 | 100 | 15 | Klugheit 

142 | Todeskuss | Normal | Status | 0 | 75 | 10 | Schönheit 

143 | Himmelsfeger | Flug | Physisch | 140 | 90 | 5 | Coolness 

144 | Wandler | Normal | Status | 0 | 100 | 10 | Klugheit 

145 | Blubber | Wasser | Spezial | 20 | 100 | 30 | Anmut 

146 | Irrschlag | Normal | Physisch | 70 | 100 | 10 | Coolness 

147 | Pilzspore | Pflanze | Status | 0 | 100 | 15 | Schönheit 

148 | Blitz | Normal | Status | 0 | 100 | 20 | Schönheit 

149 | Psywelle | Psycho | Spezial | Variiert | 80 | 15 | Klugheit 

150 | Platscher | Normal | Status | 0 | 100 | 40 | Anmut 

151 | Säurepanzer | Gift | Status | 0 | 100 | 40 | Stärke 

152 | Krabbhammer | Wasser | Physisch | 90 | 85 | 10 | Stärke 

153 | Explosion | Normal | Physisch | 250 | 100 | 5 | Schönheit 

154 | Kratzfurie | Normal | Physisch | 18 | 80 | 15 | Stärke 

155 | Knochmerang | Boden | Physisch | 50 | 90 | 10 | Stärke 

156 | Erholung | Psycho | Status | 0 | 100 | 10 | Anmut 

157 | Steinhagel | Gestein | Physisch | 75 | 90 | 10 | Stärke 

158 | Hyperzahn | Normal | Physisch | 80 | 90 | 15 | Coolness 

159 | Schärfer | Normal | Status | 0 | 100 | 30 | Anmut 

160 | Umwandlung | Normal | Status | 0 | 100 | 30 | Schönheit 

161 | Triplette | Normal | Spezial | 80 | 100 | 10 | Schönheit 

162 | Superzahn | Normal | Physisch | Variiert | 90 | 10 | Stärke 

163 | Schlitzer | Normal | Physisch | 70 | 100 | 20 | Coolness 

164 | Delegator | Normal | Status | 0 | 100 | 10 | Klugheit 

165 | Verzweifler | Normal | Physisch | 50 | 100 | unendlich | Coolness 



166 | Nachahmer | Normal | Status | 0 | 100 | 1 | Klugheit I

167 | Dreifachkick | Kampf | Physisch | 10 | 90 | 10 | Coolness I

168 | Raub | Unlicht | Physisch | 40 | 100 | 10 | Stärke I

169 | Spinnennetz | Käfer | Status | 0 | 100 | 10 | Klugheit I

170 | Willensleser | Normal | Status | 0 | 100 | 40 | Klugheit I

171 | Nachtmahr | Geist | Status | 0 | 100 | 15 | Klugheit I

172 | Flammenrad | Feuer | Physisch | 60 | 100 | 25 | Schönheit I

173 | Schnarcher | Normal | Spezial | 40 | 100 | 15 | Anmut I

174 | Fluch | Geist | Status | 0 | 100 | 10 | Stärke I

175 | Dreschflegel | Normal | Physisch | Variiert (KP) | 100 | 15 | Anmut I

176 | Umwandlung2 | Normal | Status | 0 | 100 | 30 | Schönheit I

177 | Luftstoß | Flug | Spezial | 100 | 95 | 5 | Coolness I

178 | Baumwollsaat | Pflanze | Status | 0 | 85 | 40 | Schönheit I

179 | Gegenschlag | Kampf | Physisch | Variiert (KP) | 100 | 15 | Coolness I

180 | Groll | Geist | Status | 0 | 100 | 10 | Stärke I

181 | Pulverschnee | Eis | Spezial | 40 | 100 | 25 | Schönheit I

182 | Schutzschild | Normal | Status | 0 | 100 | 10 | Anmut I

183 | Tempohieb | Kampf | Physisch | 40 | 100 | 30 | Coolness I

184 | Grimasse | Normal | Status | 0 | 90 | 10 | Stärke I

185 | Finte | Unlicht | Physisch | 60 | 100 | 20 | Klugheit I

186 | Bitterkuss | Normal | Status | 0 | 75 | 10 | Anmut I

187 | Bauchtrommel | Normal | Status | 0 | 100 | 10 | Anmut I

188 | Matschbombe | Gift | Spezial | 90 | 100 | 10 | Stärke I

189 | Lehmschelle | Boden | Spezial | 20 | 100 | 10 | Anmut I

190 | Octazooka | Wasser | Spezial | 65 | 85 | 10 | Stärke I

191 | Stachler | Boden | Status | 0 | 100 | 20 | Klugheit I

192 | Blitzkanone | Elektro | Spezial | 120 | 50 | 5 | Coolness I

193 | Gesichte | Normal | Status | 0 | 100 | 5 | Klugheit I

194 | Abgangsbund | Geist | Status | 0 | 100 | 5 | Klugheit I

195 | Abgesang | Normal | Status | 0 | 100 | 5 | Schönheit I

196 | Eissturm | Eis | Spezial | 55 | 95 | 15 | Schönheit I

197 | Scanner | Kampf | Status | 0 | 100 | 5 | Coolness I

198 | Knochenhatz | Boden | Physisch | 25 | 80 | 10 | Stärke I

199 | Zielschuss | Normal | Status | 0 | 100 | 5 | Klugheit I

200 | Wutanfall | Drache | Physisch | 120 | 100 | 15 | Coolness I

201 | Sandsturm | Gestein | Status | 0 | 100 | 10 | Stärke I

202 | Gigasauger | Pflanze | Spezial | 60 | 100 | 10 | Klugheit I

203 | Ausdauer | Normal | Status | 0 | 100 | 10 | Stärke I

204 | Charme | Normal | Status | 0 | 100 | 20 | Anmut I

205 | Walzer | Gestein | Physisch | 30 | 90 | 20 | Stärke I

206 | Trugschlag | Normal | Physisch | 40 | 100 | 40 | Coolness I

207 | Angeberei | Normal | Status | 0 | 90 | 15 | Anmut I

208 | Milchgetränk | Normal | Status | 0 | 100 | 10 | Anmut I

209 | Funkensprung | Elektro | Physisch | 65 | 100 | 20 | Coolness I

210 | Zornklinge | Käfer | Physisch | 10 | 95 | 20 | Coolness I

211 | Stahlflügel | Stahl | Physisch | 70 | 90 | 25 | Coolness I

212 | Horrorblick | Normal | Status | 0 | 100 | 5 | Schönheit I

213 | Anziehung | Normal | Status | 0 | 100 | 15 | Anmut I

214 | Schlafrede | Normal | Status | 0 | 100 | 10 | Anmut I

215 | Vitalglocke | Normal | Status | 0 | 100 | 5 | Schönheit I

216 | Rückkehr | Normal | Physisch | Variiert (Zuneigung) | 100 | 20 | Anmut I

217 | Geschenk | Normal | Physisch | Variiert (Zufall) | 90 | 15 | Anmut I

218 | Frustration | Normal | Physisch | Variiert (Zuneigung) | 100 | 20 | Anmut I

219 | Bodyguard | Normal | Status | 0 | 100 | 25 | Schönheit I

220 | Leidteiler | Normal | Status | 0 | 0 | 20 | Klugheit I

221 | Läuterfeuer | Feuer | Physisch | 100 | 95 | 5 | Schönheit I

222 | Intensität | Boden | Physisch | Variiert (Zufall) | 100 | 30 | Stärke I

223 | Wuchtschlag | Kampf | Physisch | 100 | 50 | 5 | Coolness I

224 | Vielender | Käfer | Physisch | 120 | 85 | 10 | Coolness I

225 | Feuerodem | Drache | Spezial | 60 | 100 | 20 | Coolness I

226 | Staffette | Normal | Status | 0 | 100 | 40 | Anmut I

227 | Zugabe | Normal | Status | 0 | 100 | 5 | Anmut I

228 | Verfolgung | Unlicht | Physisch | 40 | 100 | 20 | Klugheit I

229 | Turbodreher | Normal | Physisch | 20 | 100 | 40 | Coolness I

230 | Lockduft | Normal | Status | 0 | 100 | 20 | Anmut I

231 | Eisenschweif | Stahl | Physisch | 100 | 75 | 15 | Coolness I

232 | Metallklaue | Stahl | Physisch | 50 | 95 | 35 | Coolness I

233 | Überwurf | Kampf | Physisch | 70 | 100 | 10 | Coolness I

234 | Morgengrauen | Normal | Status | 0 | 100 | 5 | Schönheit I

235 | Synthese | Pflanze | Status | 0 | 100 | 5 | Klugheit I

236 | Mondschein | Normal | Status | 0 | 100 | 5 | Schönheit I

237 | Kraftreserve | Normal | Spezial | 1 | 100 | 15 | Klugheit I

238 | Kreuzhieb | Kampf | Physisch | 100 | 80 | 5 | Coolness I

239 | Windhose | Drache | Spezial | 40 | 100 | 20 | Coolness I

240 | Regentanz | Wasser | Status | 0 | 0 | 5 | Stärke I

241 | Sonnentag | Feuer | Status | 0 | 0 | 5 | Schönheit I

242 | Knirscher | Unlicht | Physisch | 80 | 100 | 15 | Stärke I

243 | Spiegelcape | Psycho | Spezial | 1 | 100 | 20 | Schönheit I

244 | Psycho-Plus | Normal | Status | 0 | 100 | 10 | Klugheit I

245 | Turbotempo | Normal | Physisch | 80 | 100 | 5 | Coolness I

246 | Antik-Kraft | Gestein | Spezial | 60 | 100 | 5 | Stärke I

247 | Spukball | Geist | Spezial | 80 | 100 | 15 | Klugheit I

248 | Seher | Psycho | Spezial | 80 | 90 | 15 | Klugheit I

249 | Zertrümmerer | Kampf | Physisch | 40 | 100 | 15 | Stärke I

250 | Whirlpool | Wasser | Spezial | 15 | 70 | 15 | Schönheit I

251 | Prügler | Unlicht | Physisch | 10 | 100 | 10 | Klugheit I

252 | Mogelhieb | Normal | Physisch | 40 | 100 | 10 | Anmut II

253 | Aufruhr | Normal | Spezial | 90 | 100 | 10 | Anmut II

254 | Horter | Normal | Status | 0 | 100 | 20 | Stärke II

255 | Entfessler | Normal | Spezial | 1 | 100 | 10 | Stärke II

256 | Verzehrer | Normal | Status | 0 | 100 | 10 | Stärke II

257 | Hitzewelle | Feuer | Spezial | 100 | 90 | 10 | Schönheit II

258 | Hagelsturm | Eis | Status | 0 | 100 | 10 | Schönheit II

259 | Folterknecht | Unlicht | Status | 0 | 100 | 15 | Stärke II

260 | Schmeichler | Unlicht | Status | 0 | 100 | 15 | Klugheit II

261 | Irrlicht | Feuer | Status | 0 | 75 | 15 | Schönheit II

262 | Memento-Mori | Unlicht | Status | 0 | 100 | 10 | Stärke II

263 | Fassade | Normal | Physisch | 70 | 100 | 20 | Anmut II

264 | Power-Punch | Kampf | Physisch | 150 | 100 | 20 | Stärke II

265 | Riechsalz | Normal | Physisch | 60 | 100 | 10 | Klugheit II

266 | Spotlight | Normal | Status | 0 | 100 | 20 | Anmut II

267 | Natur-Kraft | Normal | Status | 0 | 0 | 20 | Schönheit II

268 | Ladevorgang | Elektro | Status | 0 | 100 | 20 | Klugheit II

269 | Verhöhner | Unlicht | Status | 0 | 100 | 20 | Klugheit II

270 | Rechte Hand | Normal | Status | 0 | 100 | 20 | Klugheit II

271 | Trickbetrug | Psycho | Status | 0 | 100 | 10 | Klugheit II

272 | Rollentausch | Psycho | Status | 0 | 100 | 10 | Anmut II

273 | Wunschtraum | Normal | Status | 0 | 100 | 10 | Anmut II

274 | Zuschuss | Normal | Status | 0 | 100 | 20 | Anmut II

275 | Verwurzler | Pflanze | Status | 0 | 100 | 20 | Klugheit II

276 | Kraftkoloss | Kampf | Physisch | 120 | 100 | 5 | Stärke II

277 | Magiemantel | Psycho | Status | 0 | 100 | 15 | Schönheit II

278 | Aufbereitung | Normal | Status | 0 | 100 | 10 | Klugheit II

279 | Vergeltung | Kampf | Physisch | 60 | 100 | 10 | Stärke II

280 | Durchbruch | Kampf | Physisch | 75 | 100 | 15 | Coolness II

281 | Gähner | Normal | Status | 0 | 100 | 10 | Anmut II

282 | Abschlag | Unlicht | Physisch | 20 | 100 | 20 | Klugheit II

283 | Notsituation | Normal | Physisch | 1 | 100 | 5 | Stärke II

284 | Eruption | Feuer | Spezial | 150 | 100 | 5 | Schönheit II

285 | Wertewechsel | Psycho | Status | 0 | 100 | 10 | Klugheit II

286 | Begrenzer | Psycho | Status | 0 | 100 | 10 | Klugheit II

287 | Heilung | Normal | Status | 0 | 100 | 20 | Anmut II

288 | Nachspiel | Geist | Status | 0 | 100 | 5 | Stärke II

289 | Übernahme | Unlicht | Status | 0 | 100 | 10 | Klugheit II

290 | Geheimpower | Normal | Physisch | 70 | 100 | 20 | Klugheit II

291 | Taucher | Wasser | Physisch | 80 | 100 | 10 | Schönheit II

292 | Armstoß | Kampf | Physisch | 15 | 100 | 20 | Stärke II

293 | Tarnung | Normal | Status | 0 | 100 | 20 | Klugheit II

294 | Schweifglanz | Käfer | Status | 0 | 100 | 20 | Schönheit II

295 | Scheinwerfer | Psycho | Spezial | 70 | 100 | 5 | Klugheit II

296 | Nebelball | Psycho | Spezial | 70 | 100 | 5 | Klugheit II

297 | Daunenreigen | Flug | Status | 0 | 100 | 15 | Schönheit II

298 | Taumeltanz | Normal | Status | 0 | 100 | 20 | Anmut II

299 | Feuerfeger | Feuer | Physisch | 85 | 90 | 10 | Schönheit II

300 | Lehmsuhler | Boden | Status | 0 | 100 | 15 | Anmut II

301 | Frostbeule | Eis | Physisch | 30 | 90 | 20 | Schönheit II

302 | Nietenranke | Pflanze | Physisch | 60 | 100 | 15 | Klugheit II

303 | Tagedieb | Normal | Status | 0 | 100 | 10 | Anmut II

304 | Schallwelle | Normal | Spezial | 90 | 100 | 10 | Coolness II

305 | Giftzahn | Gift | Physisch | 50 | 100 | 15 | Klugheit II

306 | Zermalmklaue | Normal | Physisch | 75 | 95 | 10 | Coolness II

307 | Lohekanonade | Feuer | Spezial | 150 | 90 | 5 | Schönheit II

308 | Aquahaubitze | Wasser | Spezial | 150 | 90 | 5 | Schönheit II

309 | Sternenhieb | Stahl | Physisch | 100 | 85 | 10 | Coolness II

310 | Erstauner | Geist | Physisch | 30 | 100 | 15 | Klugheit II

311 | Meteorologe | Normal | Spezial | 50 | 100 | 10 | Klugheit II

312 | Aromakur | Pflanze | Status | 0 | 100 | 5 | Klugheit II

313 | Trugträne | Unlicht | Status | 0 | 100 | 20 | Klugheit II

314 | Windschnitt | Flug | Spezial | 55 | 95 | 25 | Coolness II

315 | Hitzekoller | Feuer | Spezial | 140 | 90 | 5 | Schönheit II

316 | Schnüffler | Normal | Status | 0 | 100 | 40 | Klugheit II

317 | Felsgrab | Gestein | Physisch | 50 | 80 | 10 | Klugheit II

318 | Silberhauch | Käfer | Spezial | 60 | 100 | 5 | Schönheit II

319 | Metallsound | Stahl | Status | 0 | 85 | 40 | Klugheit II

320 | Grasflöte | Pflanze | Status | 0 | 55 | 15 | Klugheit II

321 | Spaßkanone | Normal | Status | 0 | 100 | 20 | Anmut II

322 | Kosmik-Kraft | Psycho | Status | 0 | 100 | 20 | Coolness II

323 | Fontränen | Wasser | Spezial | 150 | 100 | 5 | Schönheit II

324 | Ampelleuchte | Käfer | Spezial | 75 | 100 | 15 | Schönheit II

325 | Finsterfaust | Geist | Physisch | 60 | 100 | 20 | Klugheit II

326 | Sondersensor | Psycho | Spezial | 80 | 100 | 30 | Coolness II

327 | Himmelhieb | Kampf | Physisch | 85 | 90 | 15 | Coolness II

328 | Sandgrab | Boden | Physisch | 15 | 70 | 15 | Klugheit II

329 | Eiseskälte | Eis | Spezial | K.O. | 30 | 5 | Schönheit II

330 | Lehmbrühe | Wasser | Spezial | 95 | 85 | 10 | Stärke II

331 | Kugelsaat | Pflanze | Physisch | 10 | 100 | 30 | Coolness II

332 | Aero-Ass | Flug | Physisch | 60 | 100 | 20 | Coolness II

333 | Eisspeer | Eis | Physisch | 10 | 100 | 30 | Schönheit II

334 | Eisenabwehr | Stahl | Status | 0 | 100 | 15 | Stärke II

335 | Rückentzug | Normal | Status | 0 | 100 | 5 | Anmut II

336 | Jauler | Normal | Status | 0 | 100 | 40 | Coolness II

337 | Drachenklaue | Drache | Physisch | 80 | 100 | 15 | Coolness II

338 | Fauna-Statue | Pflanze | Spezial | 150 | 90 | 5 | Coolness II

339 | Protzer | Kampf | Status | 0 | 100 | 20 | Schönheit II

340 | Sprungfeder | Flug | Physisch | 85 | 85 | 5 | Anmut II

341 | Lehmschuss | Boden | Spezial | 55 | 95 | 15 | Stärke II

342 | Giftschweif | Gift | Physisch | 50 | 100 | 25 | Klugheit II

343 | Bezirzer | Normal | Physisch | 40 | 100 | 40 | Anmut II

344 | Volttackle | Elektro | Physisch | 120 | 100 | 15 | Coolness II

345 | Zauberblatt | Pflanze | Spezial | 60 | 100 | 20 | Schönheit II

346 | Nassmacher | Wasser | Status | 0 | 100 | 15 | Anmut II

347 | Gedankengut | Psycho | Status | 0 | 100 | 20 | Klugheit II

348 | Laubklinge | Pflanze | Physisch | 90 | 100 | 15 | Coolness II

349 | Drachentanz | Drache | Status | 0 | 100 | 20 | Coolness II

350 | Felswurf | Gestein | Physisch | 25 | 80 | 10 | Stärke II

351 | Schockwelle | Elektro | Spezial | 60 | 100 | 20 | Coolness II

352 | Aquawelle | Wasser | Spezial | 60 | 100 | 20 | Schönheit II

353 | Kismetwunsch | Stahl | Spezial | 120 | 85 | 5 | Coolness II

354 | Psyschub | Psycho | Spezial | 140 | 90 | 5 | Klugheit II

355 | Ruheort | Flug | Status | 0 | 0 | 10 | Coolness V

356 | Erdanziehung | Psycho | Status | 0 | 0 | 5 | Schönheit V

357 | Wunderauge | Psycho | Status | 0 | 0 | 40 | Anmut V

358 | Weckruf | Kampf | Physisch | 60 | 100 | 10 | Klugheit V

359 | Hammerarm | Kampf | Physisch | 100 | 90 | 10 | Coolness V

360 | Gyroball | Stahl | Physisch | 1 | 100 | 5 | Schönheit V

361 | Heilopfer | Psycho | Status | 0 | 0 | 10 | Anmut V

362 | Lake | Wasser | Spezial | 65 | 100 | 10 | Klugheit V

363 | Beerenkräfte | Normal | Physisch | 1 | 100 | 15 | Coolness V

364 | Offenlegung | Normal | Physisch | 50 | 100 | 10 | Schönheit V

365 | Pflücker | Flug | Physisch | 60 | 100 | 20 | Anmut V

366 | Rückenwind | Flug | Status | 0 | 0 | 30 | Klugheit V

367 | Akupressur | Normal | Status | 0 | 0 | 30 | Coolness V

368 | Metallstoß | Stahl | Physisch | 1 | 100 | 10 | Schönheit V

369 | Kehrtwende | Käfer | Physisch | 70 | 100 | 20 | Anmut V

370 | Nahkampf | Kampf | Physisch | 120 | 100 | 5 | Klugheit V

371 | Gegenstoß | Unlicht | Physisch | 50 | 100 | 10 | Coolness V

372 | Gewissheit | Unlicht | Physisch | 50 | 100 | 10 | Schönheit V

373 | Itemsperre | Unlicht | Status | 0 | 100 | 15 | Anmut V

374 | Schleuder | Unlicht | Physisch | 1 | 100 | 10 | Stärke V

375 | Psybann | Psycho | Status | 0 | 90 | 10 | Coolness V

376 | Trumpfkarte | Normal | Spezial | 1 | 0 | 5 | Coolness V

377 | Heilblockade | Psycho | Status | 0 | 100 | 15 | Anmut V

378 | Auswringen | Normal | Spezial | 1 | 100 | 5 | Klugheit V

379 | Krafttrick | Psycho | Status | 0 | 0 | 10 | Coolness V

380 | Magensäfte | Gift | Status | 0 | 100 | 10 | Schönheit V

381 | Beschwörung | Normal | Status | 0 | 0 | 30 | Anmut V

382 | Egotrip | Normal | Status | 0 | 0 | 20 | Anmut V

383 | Imitator | Normal | Status | 0 | 0 | 20 | Coolness V

384 | Krafttausch | Psycho | Status | 0 | 0 | 10 | Schönheit V

385 | Schutztausch | Psycho | Status | 0 | 0 | 10 | Anmut V

386 | Strafattacke | Unlicht | Physisch | 60 | 100 | 5 | Klugheit V

387 | Zuflucht | Normal | Physisch | 130 | 100 | 5 | Anmut V

388 | Sorgensamen | Pflanze | Status | 0 | 100 | 10 | Schönheit V

389 | Tiefschlag | Unlicht | Physisch | 80 | 100 | 5 | Klugheit V

390 | Giftspitzen | Gift | Status | 0 | 0 | 20 | Klugheit V

391 | Statustausch | Psycho | Status | 0 | 0 | 10 | Coolness V

392 | Wasserring | Wasser | Status | 0 | 0 | 20 | Schönheit V

393 | Magnetflug | Elektro | Status | 0 | 0 | 10 | Anmut V

394 | Flammenblitz | Feuer | Physisch | 120 | 100 | 15 | Klugheit V

395 | Kraftwelle | Kampf | Physisch | 60 | 100 | 10 | Coolness V

396 | Aurasphäre | Kampf | Spezial | 90 | 0 | 20 | Schönheit V

397 | Steinpolitur | Gestein | Status | 0 | 0 | 20 | Stärke V

398 | Gifthieb | Gift | Physisch | 80 | 100 | 20 | Klugheit V

399 | Finsteraura | Unlicht | Spezial | 80 | 100 | 15 | Coolness V

400 | Nachthieb | Unlicht | Physisch | 70 | 100 | 15 | Schönheit V

401 | Nassschweif | Wasser | Physisch | 90 | 90 | 10 | Anmut V

402 | Samenbomben | Pflanze | Physisch | 80 | 100 | 15 | Klugheit V

403 | Luftschnitt | Flug | Spezial | 75 | 95 | 20 | Coolness V

404 | Kreuzschere | Käfer | Physisch | 80 | 100 | 15 | Schönheit V

405 | Käfergebrumm | Käfer | Spezial | 90 | 100 | 10 | Anmut V

406 | Drachenpuls | Drache | Spezial | 90 | 100 | 10 | Klugheit V

407 | Drachenstoß | Drache | Physisch | 100 | 75 | 10 | Coolness V

408 | Juwelenkraft | Gestein | Spezial | 70 | 100 | 20 | Schönheit V

409 | Ableithieb | Kampf | Physisch | 60 | 100 | 5 | Schönheit V

410 | Vakuumwelle | Kampf | Spezial | 40 | 100 | 30 | Klugheit V

411 | Fokusstoß | Kampf | Spezial | 120 | 70 | 5 | Coolness V

412 | Energieball | Pflanze | Spezial | 80 | 100 | 10 | Schönheit V

413 | Sturzflug | Flug | Physisch | 120 | 100 | 15 | Anmut V

414 | Erdkräfte | Boden | Spezial | 90 | 100 | 10 | Klugheit V

415 | Wechseldich | Unlicht | Status | 0 | 100 | 10 | Coolness V

416 | Gigastoß | Normal | Physisch | 150 | 90 | 5 | Schönheit V

417 | Ränkeschmied | Unlicht | Status | 0 | 0 | 20 | Anmut V

418 | Patronenhieb | Stahl | Physisch | 40 | 100 | 30 | Klugheit V

419 | Lawine | Eis | Physisch | 60 | 100 | 10 | Coolness V

420 | Eissplitter | Eis | Physisch | 40 | 100 | 30 | Schönheit V

421 | Dunkelklaue | Geist | Physisch | 70 | 100 | 15 | Anmut V

422 | Donnerzahn | Elektro | Physisch | 65 | 95 | 15 | Klugheit V

423 | Eiszahn | Eis | Physisch | 65 | 95 | 15 | Coolness V

424 | Feuerzahn | Feuer | Physisch | 65 | 95 | 15 | Schönheit V

425 | Schattenstoß | Geist | Physisch | 40 | 100 | 30 | Klugheit V

426 | Schlammbombe | Boden | Spezial | 65 | 85 | 10 | Klugheit V

427 | Psychoklinge | Psycho | Physisch | 70 | 100 | 20 | Coolness V

428 | Zen-Kopfstoß | Psycho | Physisch | 80 | 90 | 15 | Schönheit V

429 | Spiegelsalve | Stahl | Spezial | 65 | 85 | 10 | Anmut V

430 | Lichtkanone | Stahl | Spezial | 80 | 100 | 10 | Klugheit V

431 | Kraxler | Normal | Physisch | 90 | 85 | 20 | Coolness V

432 | Auflockern | Flug | Status | 0 | 0 | 15 | Schönheit V

433 | Bizarroraum | Psycho | Status | 0 | 0 | 5 | Anmut V

434 | Draco Meteor | Drache | Spezial | 140 | 90 | 5 | Klugheit V

435 | Ladungsstoß | Elektro | Spezial | 80 | 100 | 15 | Coolness V

436 | Flammensturm | Feuer | Spezial | 80 | 100 | 15 | Stärke V

437 | Blättersturm | Pflanze | Spezial | 140 | 90 | 5 | Anmut V

438 | Blattgeißel | Pflanze | Physisch | 120 | 85 | 10 | Schönheit V

439 | Felswerfer | Gestein | Physisch | 150 | 90 | 5 | Stärke V

440 | Giftstreich | Gift | Physisch | 70 | 100 | 20 | Coolness V

441 | Mülltreffer | Gift | Physisch | 120 | 70 | 5 | Coolness V

442 | Eisenschädel | Stahl | Physisch | 80 | 100 | 15 | Stärke V

443 | Magnetbombe | Stahl | Physisch | 60 | 0 | 20 | Coolness V

444 | Steinkante | Gestein | Physisch | 100 | 80 | 5 | Stärke V

445 | Liebreiz | Normal | Status | 0 | 100 | 20 | Schönheit V

446 | Tarnsteine | Gestein | Status | 0 | 0 | 20 | Coolness V

447 | Strauchler | Pflanze | Spezial | 1 | 100 | 20 | Klugheit V

448 | Geschwätz | Flug | Spezial | 60 | 100 | 20 | Klugheit V

449 | Urteilskraft | Normal | Spezial | 100 | 100 | 10 | Klugheit V

450 | Käferbiss | Käfer | Physisch | 60 | 100 | 20 | Stärke V

451 | Ladestrahl | Elektro | Spezial | 50 | 90 | 10 | Schönheit V

452 | Holzhammer | Pflanze | Physisch | 120 | 100 | 15 | Stärke V

453 | Wasserdüse | Wasser | Physisch | 40 | 100 | 20 | Schönheit V

454 | Schlagbefehl | Käfer | Physisch | 90 | 100 | 15 | Klugheit V

455 | Blockbefehl | Käfer | Status | 0 | 0 | 10 | Klugheit V

456 | Heilbefehl | Käfer | Status | 0 | 0 | 10 | Klugheit V

457 | Kopfstoß | Gestein | Physisch | 150 | 80 | 5 | Stärke V

458 | Doppelschlag | Normal | Physisch | 35 | 90 | 10 | Klugheit V

459 | Zeitenlärm | Drache | Spezial | 150 | 90 | 5 | Coolness V

460 | Raumschlag | Drache | Spezial | 100 | 95 | 5 | Stärke V

461 | Lunartanz | Psycho | Status | 0 | 0 | 10 | Schönheit V

462 | Quetschgriff | Normal | Physisch | Variiert | 100 | 5 | Stärke V

463 | Lavasturm | Feuer | Spezial | 120 | 70 | 5 | Stärke V

464 | Schlummerort | Unlicht | Status | 0 | 80 | 10 | Klugheit V

465 | Schocksamen | Pflanze | Spezial | 120 | 85 | 5 | Coolness V

466 | Unheilböen | Geist | Spezial | 60 | 100 | 5 | Klugheit V

467 | Schemenkraft | Geist | Physisch | 120 | 100 | 5 | Klugheit | Gen. IV

468 | Klauenwetzer | Unlicht | Status | 0 | 100 | 15 | - |Gen. V

469 | Rundumschutz | Gestein | Status | 0 | 100 | 10 | - |Gen. V

470 | Schutzteiler | Psycho | Status | 0 | 100 | 10 | - |Gen. V

471 | Kraftteiler | Psycho | Status | 0 | 100 | 10 | - |Gen. V

472 | Wunderraum | Psycho | Status | 0 | 100 | 10 | - |Gen. V

473 | Psychoschock | Psycho | Spezial | 80 | 100 | 10 | - |Gen. V

474 | Giftschock | Gift | Spezial | 65 | 100 | 10 | - |Gen. V

475 | Autotomie | Stahl | Status | 0 | 100 | 15 | - |Gen. V

476 | Wutpulver | Käfer | Status | 0 | 100 | 20 | - |Gen. V

477 | Telekinese | Psycho | Status | 0 | 100 | 15 | - |Gen. V

478 | Magieraum | Psycho | Status | 0 | 100 | 10 | - |Gen. V

479 | Katapult | Gestein | Physisch | 50 | 100 | 15 | - |Gen. V

480 | Bergsturm | Kampf | Physisch | 40 | 100 | 10 | - |Gen. V

481 | Funkenflug | Feuer | Spezial | 70 | 100 | 15 | - |Gen. V

482 | Schlammwoge | Gift | Spezial | 95 | 100 | 15 | - |Gen. V

483 | Falterreigen | Käfer | Status | 0 | 100 | 20 | - |Gen. V

484 | Rammboss | Stahl | Physisch | 1 | 100 | 10 | - |Gen. V

485 | Synchrolärm | Psycho | Spezial | 70 | 100| 15 | - |Gen. V

486 | Elektroball | Elektro | Spezial | 1 | 100 | 10 | - |Gen. V

487 | Überflutung | Wasser | Status | 0 | 100 | 20 | - |Gen. V

488 | Nitroladung | Feuer | Physisch | 50 | 100 | 20 | - |Gen. V

489 | Einrollen | Gift | Status | 0 | 100 | 20 | - |Gen. V

490 | Fußtritt | Kampf | Physisch | 60 | 100 | 20 | - |Gen. V

491 | Säurespeier | Gift | Spezial | 40 | 100 | 20 | - |Gen. V

492 | Schmarotzer | Unlicht | Physisch | 95 | 100| 15 | - |Gen. V

493 | Wankelstrahl | Normal | Status | 0 | 100 | 15 | - |Gen. V

494 | Zwango | Normal | Status | 0 | 100 | 15 | - |Gen. V

495 | Galanterie | Normal | Status | 0 | 100 | 15 | - |Gen. V

496 | Kanon | Normal | Spezial | 60 | 100 | 15 | - |Gen. V

497 | Widerhall | Normal | Spezial | 40 | 100 | 15 | - |Gen. V

498 | Zermürben | Normal | Physisch | 70 | 100|20 | - |Gen. V

499 | Klärsmog | Gift | Spezial | 50 | 100 | 15 | - |Gen. V

500 | Kraftvorrat | Psycho | Spezial | 20 | 100 | 10 | - |Gen. V
   
501 | Rapidschutz | Kampf | Status | 0 | 100<sup>?</sup> | 15 | - |Gen. V
   
502 | Seitentausch | Psycho | Status | 0 | 100 | 15 | - |Gen. V
   
503 | Siedewasser | Wasser | Spezial | 80 | 100 | 15 | - |Gen. V
   
504 | Hausbruch | Normal | Status | 0 | 100 | 15 | - |Gen. V
   
505 | Heilwoge | Psycho | Status | 0 | 100 | 10 | - |Gen. V
   
506 | Bürde | Geist | Spezial | 50 | 100 | 10 | - |Gen. V
   
507 | Freier Fall | Flug | Physisch | 60 | 100 | 10 | - |Gen. V
   
508 | Gangwechsel | Stahl | Status | 0 | 100 | 10 | - |Gen. V
   
509 | Überkopfwurf | Kampf | Physisch | 60 | 90 | 10 | - |Gen. V
   
510 | Einäschern | Feuer | Spezial | 30 | 100 | 15 | - |Gen. V
   
511 | Verzögerung | Unlicht | Status | 0 | 100 | 15 | - |Gen. V
   
512 | Akrobatik | Flug | Physisch | 55 | 100 | 15 | - |Gen. V
   
513 | Typenspiegel | Normal | Status | 0 | 100 | 15 | - |Gen. V

514 | Heimzahlung | Normal | Physisch | 70 | 100 | 5 | - |Gen. V
   
515 | Wagemut | Kampf | Spezial | 1 | 100 | 5 | - |Gen. V
   
516 | Offerte | Normal | Status | 0 | 100 | 15 | - |Gen. V
   
517 | Inferno | Feuer | Spezial | 100 | 50 | 5 | - |Gen. V
   
518 | Wassersäulen | Wasser | Spezial | 50 | 100 | 10 | - |Gen. V
   
519 | Feuersäulen | Feuer | Spezial | 50 | 100 | 10 | - |Gen. V
   
520 | Pflanzsäulen | Pflanze | Spezial | 50 | 100 | 10 | - |Gen. V
   
521 | Voltwechsel | Elektro | Spezial | 70 | 100 | 20 | - |Gen. V

522 | Käfertrutz | Käfer | Spezial | 30 | 100 | 20 | - |Gen. V
   
523 | Dampfwalze | Boden | Physisch | 60 | 100 | 20 | - |Gen. V
   
524 | Eisesodem | Eis | Spezial | 40 | 90 | 10 | - |Gen. V
   
525 | Drachenrute | Drache | Physisch | 60 | 90 | 10 | - |Gen. V
   
526 | Kraftschub | Normal | Status | 0 | 100 | 30 | - |Gen. V
   
527 | Elektronetz | Elektro | Spezial | 55 | 95 | 15 | - |Gen. V
   
528 | Stromstoß | Elektro | Physisch | 90 | 100 | 15 | - |Gen. V
   
529 | Schlagbohrer | Boden | Physisch | 80 | 95 | 10 | - |Gen. V
   
530 | Doppelhieb | Drache | Physisch | 40 | 90 | 15 | - |Gen. V

531 | Herzstempel | Psycho | Physisch | 60 | 100 | 25 | - |Gen. V
   
532 | Holzgeweih | Pflanze | Physisch | 75 | 100 | 10 | - |Gen. V
   
533 | Sanctoklinge | Kampf | Physisch | 90 | 100 | 20 | - |Gen. V
   
534 | Kalkklinge | Wasser | Physisch | 75 | 90 | 10 | - |Gen. V
   
535 | Brandstempel | Feuer | Physisch | 1 | 100 | 10 | - |Gen. V

536 | Grasmixer | Pflanze | Spezial | 65 | 90 | 10 | - |Gen. V
   
537 | Quetschwalze | Käfer | Physisch | 65 | 100 | 20 | - |Gen. V
   
538 | Watteschild | Pflanze | Status | 0 | 100 | 10 | - |Gen. V
   
539 | Nachtflut | Unlicht | Spezial | 85 | 95 | 10 | - |Gen. V
   
540 | Psychostoß | Psycho | Spezial | 100 | 100 | 10 | - |Gen. V
   
541 | Kehrschelle | Normal | Physisch | 25 | 85 | 10 | - |Gen. V
   
542 | Orkan | Flug | Spezial | 120 | 70 | 10 | - |Gen. V
   
543 | Steinschädel | Normal | Physisch | 120 | 100 | 15 | - |Gen. V
   
544 | Klikkdiskus | Stahl | Physisch | 50 | 85 | 15 | - |Gen. V
   
545 | Flammenball | Feuer | Spezial | 100 | 100 | 5 | - |Gen. V
   
546 | Techblaster | Normal | Spezial | 85 | 100 | 5 | - |Gen. V
   
547 | Urgesang | Normal | Spezial | 75 | 100 | 10 | - |Gen. V
   
548 | Mystoschwert | Kampf | Spezial | 85 | 100 | 10 | - |Gen. V
   
549 | Eiszeit | Eis | Spezial | 65 | 95 | 10 | - |Gen. V
   
550 | Blitzschlag | Elektro | Physisch | 130 | 85 | 5 | - |Gen. V
   
551 | Blauflammen | Feuer | Spezial | 130 | 85 | 5 | - |Gen. V
   
552 | Feuerreigen | Feuer | Spezial | 80 | 100 | 10 | - |Gen. V
   
553 | Frostvolt | Eis | Physisch | 140 | 90 | 5 | - |Gen. V
   
554 | Frosthauch | Eis | Spezial | 140 | 90 | 5 | - |Gen. V
   
555 | Standpauke | Unlicht | Spezial | 55 | 95 | 15 | - |Gen. V
   
556 | Eiszapfhagel | Eis | Physisch | 85 | 90 | 10 | - |Gen. V
   
557 | V-Generator | Feuer | Physisch | 180 | 95 | 5 | - |Gen. V
   
558 | Kreuzflamme | Feuer | Spezial | 100 | 100 | 5 | - |Gen. V
   
559 | Kreuzdonner | Elektro | Physisch | 100 | 100 | 5 | - |Gen. V
    */
};