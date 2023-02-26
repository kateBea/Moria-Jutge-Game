# Mòria

Repositori del Joc d'EDA

## Introducció

Es tracta d'un joc per a quatre jugadors, identificat amb números de 0 a 3. Cada
jugador té el control d'un clan de nans ajudats per alguns mags. L'objectiu del
joc és dominar l’antic regne de Mòria.

Aquest joc també inclou algunes unitats de Sauron: orcs, trolls i un Balrog. Les
unitats de Sauron corresponen al jugador -1. El joc té una durada de 200 rondes,
numerades de l'1 al 200. Cada unitat de cada jugador, les unitats de Sauron incloses,
pot moure's com a màxim una vegada per ronda.

El tauler té unes dimensions de 60 x 60. Les unitats no es poden moure fora
d'ell. Les 2 files superiors, les 2 files inferiors, les 2 columnes de més a
l’esquerra i les dues columnes de més a la dreta són cel·les fora de Mòria
i, per tant, s'on les  úniques cel·les de tipus Exterior. La resta de cel·les,
de tipus Cova, Abisme, Granit i Roca, pertanyen a Mòria.

Les cel·les de tipus Cova són transitables per qualsevol unitat. Els Abismes són
cel·les nom ́es transitables per orcs i Balrog. El Granit i les cel·les de tipus Roca
nom ́es s  ́on transitables per Balrog. Tot i això, les cel·les de tipus Roca són prou
toves per ser excavades pels nans i convertides en Coves. Inicialment, la majoria
de cel·les de M `oria s  ́on del tipus Roca i Cova. Per defecte, 80 cel·les de tipus
Cova tenen un tresor. Els nans hauran d’excavar per assolir aquests tresors. També
hi ha algunes cel·les de Granit. Els Abismes només apareixen, amb una probabilitat
petita, despr ́es que les cel·les de tipus Roca siguin excavades per nans.

Cada cel·la pot tenir com a màxim una unitat. Quan una unitat intenta moure's
a una cel·la que ja ocupava una altra unitat, això significa un atac. Una unitat
que ataca no es mou durant aquesta ronda, encara que la unitat atacada sigui
morta. Això inclou "atacar" una cel·la de tipus Roca (vegeu més avall).

Els nans i els mags no poden atacar unitats del mateix clan. (Aixè seria un
moviment il·legal.) Els orcs i trolls mai ataquen altres unitats de Sauron. El
Balrog no ataca: nom ́es mata tot el que l’envolta.

Els nans sén els personatges principals d’aquest joc. Al principi, cada clan
en t ́e 20. Els nans neixen amb 100 punts de salut. Quan ataquen, la unitat
enemiga contigua perd entre 20 i 40 punts de salut. Els nans es poden moure
horitzontalment, verticalment i en diagonal a les cel·les Exteriors i les de tipus Cova.

Captura de partida de dummies:

![Demo picture](/info/demo.png)

## Jugadors implementats

- [Kate](src/Game/AIKate.cc) 
- [UserIsTaken](src/Game/AIUserIsTaken.cc)
