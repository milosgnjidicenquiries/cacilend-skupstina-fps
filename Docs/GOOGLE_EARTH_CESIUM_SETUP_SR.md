# Google Photorealistic 3D Tiles u Unreal Engine 5.8

Ovo je opcioni dodatak glavnom UE5 FPS projektu. Geometrija zgrada i parka ostaje dostupna kao offline fallback; Google 3D tiles se učitavaju uživo i ne kopiraju se u projekat.

## Koordinate nivoa

UE5 svet koristi lokalni origin tako da `(0, 0, 0)` odgovara približno `44.81076, 20.46583, 160 m` ellipsoidne visine:

| Objekat | UE5 lokalna pozicija (cm) | Približna lokacija |
| --- | ---: | --- |
| Dom Narodne skupštine | `(0, 10150, 0)` | `44.81167, 20.46583` |
| Pionirski park | `(-11200, -2850, 0)` | `44.81050, 20.46440` |
| Predsedništvo / Novi dvor | `(-21000, -9550, 0)` | `44.80990, 20.46317` |
| Palata Glavne pošte | `(12300, -1750, 0)` | `44.81060, 20.46740` |

Ovo su okvirne, lokalno skalirane koordinate za igranje — nisu geodetski survey. Proveri vizuelno poravnanje na svom Unreal računaru pre nego što ukloniš offline geometriju.

## Jednokratno podešavanje

1. U [Google Cloud Console](https://console.cloud.google.com/) kreiraj projekat.
2. Omogući **Map Tiles API** i podesi billing. Google obračunava upotrebu po SKU-u, uključujući Photorealistic 3D Tiles; namesti kvote i obaveštenja o potrošnji pre gameplay testiranja.
3. Kreiraj API key i ograniči ga na odgovarajući API i aplikaciju koliko god dozvoljava desktop workflow.
4. Instaliraj verziju **Cesium for Unreal** kompatibilnu sa Unreal Engine 5.8. U Unreal Editor-u izaberi **Edit → Plugins**, pronađi *Cesium for Unreal*, uključi plugin i restartuj editor.
5. U korenu projekta napravi folder `Config/Private`.
6. U lokalni, git-ignored fajl `Config/Private/GoogleMapsApiKey.txt` stavi samo API key (bez dodatnog teksta). Alternativa je Windows environment variable `GOOGLE_MAPS_API_KEY`.
7. Pokreni `Setup_Cacilend_Assets.bat`. Skripta će pokušati da pronađe Cesium plugin i API key; ako ih nađe, kreira/konfiguriše Cesium Georeference i Google 3D Tileset. Ako nije moguće podesiti property automatski, proveri izlazni Unreal log.
8. Proveri da je georeference origin podešen na Latitude `44.81076`, Longitude `20.46583`, Height približno `160 m` (visina iznad elipsoida; početna aproksimacija). To poravnava postojeći offline svet.
9. U World Outliner-u izaberi `GooglePhotorealistic3DTiles_Belgrade`, potvrdi **Source = From URL**, URL `https://tile.googleapis.com/v1/3dtiles/root.json?key=YOUR_API_KEY` i uključi **Show Credits On Screen**.
10. Pritisni Play i proveri da tileset učita centar Beograda. Skripta pokušava da uključi **Create Physics Meshes** da bi FPS karakter mogao da se oslanja na učitane tiles. Ovo troši dodatne resurse; ako je performansa loša, smanji nivo geometrije/obim tileset-a pre nego što isključiš kolizije. Ako ne radi, proveri log, ključ, API/billing, mrežu i Cesium plugin verziju.
11. Podrazumevano UE5 ostavlja offline geometriju uključenu čak i ako postoji Cesium tileset actor — to je namerno, da ne dobiješ praznu mapu ako tiles ne mogu da se učitaju. Kada si vizuelno proverio da je Google fotogrametrija stvarno dostupna i da joj je atribucija vidljiva, možeš pokrenuti editor/EXE sa argumentom `-CacilendUseGooglePhotogrammetry` da sakriješ proceduralne proxy zgrade i ostaviš live tiles kao glavnu podlogu. Nemoj uključivati ovaj argument pre nego što potvrdiš da pločice učitavaju tvoj deo Beograda.

## Važno ograničenje za EEA billing

Google trenutno navodi da Photorealistic 3D Tiles nisu dostupni projektima povezanim sa billing nalogom čija je adresa u Evropskom ekonomskom prostoru (EEA). Ako je Google Cloud billing profil registrovan u Hrvatskoj ili drugoj EEA zemlji, nemoj računati da će se ovaj sloj učitati; Google može vratiti HTTP 403 čak i kada je API ključ ispravan. Proveri zvanične [Google 3D Tiles error-handling napomene](https://developers.google.com/maps/documentation/tile/error_handling) pre uključivanja billing-a. Ako billing profil nije EEA, ipak su neophodni API pristup, ključ, Map Tiles API i billing.

Ako Photorealistic 3D Tiles nisu dostupni, offline geometrija u projektu ostaje aktivna. Alternativa je koristiti zasebne, legalno licencirane lokalne modele zgrada i open map podloge; nemoj preuzimati ili pakovati Google Earth fotogrametriju van uslova Google licence.

## Obavezna atribucija i ključevi

Google-ova pravila traže da prikazuješ kredite/attribution koji se dobijaju sa tiles. Visina u Cesium-u je ellipsoidna, ne isto što i lokalno prijavljena nadmorska visina; vrednost 160 m je okvirni start i mora vizuelno da se proveri. Zbog toga skripta postavlja `show_credits_on_screen = True`. Nemoj ga isključivati. Google API key nije upisan u source code; fajl sa ključem je git-ignored. Ipak, ako je ključ upotrebljen u tileset URL-u unutar projekta/pakovanog EXE-a, može se izvući iz klijentske aplikacije. Za lokalni test koristi poseban, API-ograničen ključ, postavi kvote i budžetska upozorenja i ne distribuiraj build sa ključem bez provere Google pravila/bezbednosti. Ne postavljaj key u javni GitHub, screenshot ili javni build bez pravilnih ograničenja.

Google zvanična uputstva:
- [Map Tiles API overview](https://developers.google.com/maps/documentation/tile/overview)
- [Work with a 3D Tiles renderer — Cesium for Unreal](https://developers.google.com/maps/documentation/tile/use-renderer)
- [Usage and billing](https://developers.google.com/maps/documentation/tile/usage-and-billing)
- [Map Tiles API policies / attribution](https://developers.google.com/maps/documentation/tile/policies)

Live tiles nisu lokalni originalni COD2 modeli; to je fotogrametrijska podloga stvarnog grada. Kod offline testiranja ostaju proceduralni, približni landmark modeli.
