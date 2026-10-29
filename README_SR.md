# Ćacilend: Skupština FPS — glavni UE5 projekat

**Glavni proizvod je Windows FPS u Unreal Engine 5.8.** Browser/Render verzija je prethodni demo, sačuvan u arhivi i ostavljen online radi poređenja.

## Šta je u UE5 projektu

- `CacilendUE.uproject` — Unreal Engine 5.8 C++ projekat.
- `Source/CacilendUE/` — FPS karakter, oružje, protivnici i generator scene.
- `Content/Python/Setup_Cacilend.py` — priprema mape, materijala, tekstura i opcionalnih uvezenih modela.
- `Content/Python/Import_Cacilend_Assets.py` — uvoz FBX statičnih mesh-eva, skeletal mesh-eva, animacija i tekstura.
- `Content/Python/Setup_Google3DTiles_Cesium.py` — opciono podešavanje Google Photorealistic 3D Tiles preko Cesium for Unreal.
- `Scripts/generate_textures.py` — generiše lokalne PBR teksture.
- `Scripts/extract_cod2_assets.py` / `Extract_COD2_Assets.bat` — lokalno izvlačenje relevantnih `.iwd` sadržaja iz COD2 instalacije.
- `Check_Cacilend_UE5_Requirements.bat` — automatska Windows/UE/Visual Studio/MSVC/SDK/hardverska provera pre build-a; pravi `Build/UE5-preflight-report.txt`.
- `Scripts/Check_Cacilend_UE5_Requirements.ps1` — implementacija dijagnostike.
- `Build_Run_Cacilend_UE5.bat` — pokušava stvarnu UE5.8 C++ kompilaciju, zatim pokreće setup mape i asseta u editoru.
- `Import_Cacilend_Assets.bat`, `Setup_Cacilend_Assets.bat`, `Package_Windows.bat` — Windows alati.
- `Install_COD2_Blender_Importer.bat` — preuzima gotov COD2 Blender importer za D3DBSP mape i XModel-e.
- `Convert_COD2_Animations.bat` — konvertuje lokalnu `.xanim_bin` animaciju u XAnim export tekst.
- `Install_COD2_Animation_Addon.bat` — priprema odvojeni Blender dodatak za XAnim; njegov uvoz je eksperimentalan i mora se proveriti po konkretnom fajlu.
- `Extract_COD2_Assets.bat` i `Download_CC0_Weapon_Fallback.bat` — pripremaju lokalne assete / privremenu pušku.

UE5 ima offline fallback geometriju za Dom Narodne skupštine, Pionirski park, Predsedništvo/Novi dvor, Palatu Glavne pošte, okolne blokove i kamp. Ako odgovarajući uvezeni asset postoji na dokumentovanoj `/Game/Cacilend/Imported/...` putanji, map builder/karakter/oružje će ga koristiti. Bez lokalno uvezenih modela projekat ostaje proceduralni prototip.

## Pokretanje na Windows računaru

**Preporučeno:** Windows 11 64-bit, 32 GB RAM-a i grafička sa 8 GB VRAM-a i DirectX 12. UE5.8 je namenjen i podržanoj 64-bitnoj verziji Windowsa 10, ali Windows 11 je preporuka. Zahtevi za ugodan rad su veći od minimuma za samo pokretanje editora.

Za C++ build koristi Visual Studio 2026 (aktuelna Epic preporuka) ili kompatibilni Visual Studio 2022 17.14+. U Visual Studio Installer-u uključi workload `Game development with C++`, MSVC toolset 14.38 ili noviji i Windows SDK 10.0.22621.0 ili noviji. U opcionalnim komponentama workload-a uključi Visual Studio Tools for Unreal Engine; Blueprint debugger i Unreal Engine Test Adapter su korisni dodatni alati.

1. Preuzmi/kloniraj repo u kratak folder, npr. `C:\Games\CacilendUE`.
2. Pokreni `Check_Cacilend_UE5_Requirements.bat`. Skripta proverava Windows, RAM, GPU naziv, UE5.8, Visual Studio/C++ workload, MSVC, Windows SDK i slobodan prostor. Izveštaj se čuva u `Build/UE5-preflight-report.txt`. Reši sve `FAIL` stavke pre build-a; `WARN` stavke označavaju moguća ograničenja.
3. Pokreni `Build_Run_Cacilend_UE5.bat`. Skripta ponavlja proveru preduslova automatski, zatim poziva UnrealBuildTool za `CacilendUEEditor Win64 Development` i tek ako kompilacija uspe pokreće setup u Unreal Editor-u. Pronalazač pretražuje poznate lokacije i Epic Launcher manifeste; ako instalacija ipak nije pronađena, podesi `UE_5_8_ROOT` na folder engine-a.
4. U Editor-u otvori `/Game/Maps/L_Cacilend` i pritisni **Play**.

**Kontrole:** WASD kretanje, miš gledanje, levi klik pucanje, R punjenje, Shift sprint, Ctrl čučanj (držati), Space skok, E interakcija/ulazak/izlazak, Esc pauza/nastavak.

Za detalje o lokalnom build-u, dijagnostičkim logovima i opcionalnom self-hosted GitHub pakovanju vidi [Docs/UE5_WINDOWS_BUILD_SR.md](Docs/UE5_WINDOWS_BUILD_SR.md).
5. Tek kada je gameplay potvrđen u Editor-u, pokreni `Package_Windows.bat` za Win64 Shipping paket. Pakovanje sada odbija da krene ako nema `Build/UE5-setup-status.txt` sa statusom `SUCCESS` ili ako `Content/Maps/L_Cacilend.umap` ne postoji.

Ako C++ build ne uspe, ceo log se čuva u `Build/UE5-build.log`; ako Windows pakovanje ne uspe, UAT log se čuva u `Build/UE5-package.log`. Skripte prikažu i završne linije greške. Poruka iz dijagnostičke skripte nije dokaz da se projekat kompajlira. GitHub CI proverava strukturu, sintaksu i veze između konfiguracije i koda, ali ne kompajlira UE C++ niti potvrđuje EXE/gameplay.

## Originalni COD2 modeli i teksture

Za privremeni fallback postoji `Download_CC0_Weapon_Fallback.bat`, koji preuzima CC0 pušku iz zasebnog open-source paketa; to nije originalni COD2 model. Javni repo ne sadrži originalne Call of Duty 2 IWD arhive niti izvučene vlasničke assete. Za lokalnu instalaciju koju poseduješ pripremljeni su ekstrakcioni alat, XModel/map importer i odvojena (eksperimentalna) putanja za XAnim animacije. Pročitaj `Docs/ASSET_IMPORT_PIPELINE_SR.md`. Očekivani modeli su `SM_NationalAssembly`, `SM_PionirskiPark`, `SM_Presidency`, `SM_MainPostOfficePalace`, `SK_COD2_Soldier` i `SM_COD2_Weapon`, uz opcione `AN_COD2_Idle` / `AN_COD2_Walk` animacije. Datoteke moraš stvarno lokalno izvesti i uvesti pre nego što zamene proceduralne modele.

## Google Earth / Photorealistic 3D Tiles

Fotogrametrija uživo je opciona: zahteva Cesium for Unreal, omogućen Google Maps Platform Map Tiles API, projekat sa billing-om, ograničen API key i vidljivu atribuciju. Lokalno čuvaj ključ u `Config/Private/GoogleMapsApiKey.txt`; fajl je isključen iz Git-a. Pogledaj `Docs/GOOGLE_EARTH_CESIUM_SETUP_SR.md`. Offline mapa ostaje dostupna i bez Cesium-a/ključa.

## Arhiva starih demo verzija

- [Snapshot grana: archive/browser-render-demo-2026-10-29](https://github.com/milosgnjidicenquiries/cacilend-skupstina-fps/tree/archive/browser-render-demo-2026-10-29)
- [Uputstvo za arhivu](Archive/README_SR.md)
- [Browser demo na Renderu](https://cacilend-skupstina-browser.onrender.com)
- [Browser 3D galerija modela](https://cacilend-skupstina-browser.onrender.com/assets.html)

Folder `browser/` namerno ostaje u repo-u zato što ga Render objavljuje kao statički sajt. Ne premeštaj ga bez promene Render konfiguracije.

## Status validacije

GitHub Actions proverava JSON, sintaksu PowerShell/Python/JavaScript, strukturnu ravnotežu C++, kao i projektne ugovore između inputa, GameMode-a, HUD-a, mape, setup-a i launcher skripti. To i dalje **nije** stvarna C++ kompilacija niti gameplay test. Repo još nema verifikovan Windows EXE; UnrealBuildTool, editor setup, pakovanje i testiranje moraju da se urade na Windows računaru sa instaliranim UE 5.8.