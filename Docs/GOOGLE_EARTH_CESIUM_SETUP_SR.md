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
4. Instaliraj verziju **Cesium for Unreal** kompatibilnu sa instaliranim Unreal Engine 5.8, restartuj editor i uključi plugin ako UE to traži.
5. U korenu projekta napravi folder `Config/Private`.
6. U lokalni, git-ignored fajl `Config/Private/GoogleMapsApiKey.txt` stavi samo API key (bez dodatnog teksta). Alternativa je Windows environment variable `GOOGLE_MAPS_API_KEY`.
7. Pokreni `Setup_Cacilend_Assets.bat`. Skripta će pokušati da pronađe Cesium plugin i API key; ako ih nađe, kreira/konfiguriše Cesium Georeference i Google 3D Tileset. Ako nije moguće podesiti property automatski, proveri izlazni Unreal log.
8. Proveri da je georeference origin podešen na Latitude `44.81076`, Longitude `20.46583`, Height približno `160 m` (visina iznad elipsoida; početna aproksimacija). To poravnava postojeći offline svet.
9. U World Outliner-u izaberi `GooglePhotorealistic3DTiles_Belgrade`, potvrdi **Source = From URL**, URL `https://tile.googleapis.com/v1/3dtiles/root.json?key=YOUR_API_KEY` i uključi **Show Credits On Screen**.
10. Pritisni Play i proveri da tileset učita centar Beograda. Ako ne radi, proveri log, ključ, API/ billing, mrežu i Cesium plugin verziju.

## Obavezna atribucija i ključevi

Google-ova pravila traže da prikazuješ kredite/attribution koji se dobijaju sa tiles. Visina u Cesium-u je ellipsoidna, ne isto što i lokalno prijavljena nadmorska visina; vrednost 160 m je okvirni start i mora vizuelno da se proveri. Zbog toga skripta postavlja `show_credits_on_screen = True`. Nemoj ga isključivati. Google API key nije upisan u source code; fajl sa ključem je git-ignored. Ne postavljaj key u javni GitHub, screenshot ili javni build bez pravilnih ograničenja.

Google zvanična uputstva:
- [Map Tiles API overview](https://developers.google.com/maps/documentation/tile/overview)
- [Work with a 3D Tiles renderer — Cesium for Unreal](https://developers.google.com/maps/documentation/tile/use-renderer)
- [Usage and billing](https://developers.google.com/maps/documentation/tile/usage-and-billing)
- [Map Tiles API policies / attribution](https://developers.google.com/maps/documentation/tile/policies)

Live tiles nisu lokalni originalni COD2 modeli; to je fotogrametrijska podloga stvarnog grada. Kod offline testiranja ostaju proceduralni, približni landmark modeli.
