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
- `Import_Cacilend_Assets.bat`, `Setup_Cacilend_Assets.bat`, `Package_Windows.bat` — Windows alati.

UE5 ima offline fallback geometriju za Dom Narodne skupštine, Pionirski park, Predsedništvo/Novi dvor, Palatu Glavne pošte, okolne blokove i kamp. Ako odgovarajući uvezeni asset postoji na dokumentovanoj `/Game/Cacilend/Imported/...` putanji, map builder/karakter/oružje će ga koristiti. Bez lokalno uvezenih modela projekat ostaje proceduralni prototip.

## Pokretanje na Windows računaru

Potrebno: Windows 10/11 64-bit, Unreal Engine 5.8, Visual Studio 2022 sa `Desktop development with C++` i `Game development with C++` workload-ima i kompatibilan Windows SDK.

1. Preuzmi/kloniraj repozitorijum u kratak folder, npr. `C:\\Games\\CacilendUE`.
2. Otvori `CacilendUE.uproject` i dozvoli Unreal/Visual Studio-u da kompajlira C++ modul.
3. (Opciono) Za modele iz svoje legalne lokalne COD2 instalacije, pročitaj `Docs/ASSET_IMPORT_PIPELINE_SR.md`, pokreni `Extract_COD2_Assets.bat`, uvezi željene modele u Blender, izvezi ih kao FBX i sačuvaj ih u `SourceArt/Import/`.
4. Pokreni `Setup_Cacilend_Assets.bat`. Skripta generiše PBR teksture, uvozi dostupne lokalne FBX/teksture, napravi materijale/mapu i pokuša opciono Google 3D Tiles podešavanje.
5. U Unreal Editor-u otvori `/Game/Maps/L_Cacilend` i pritisni **Play**.
6. Posle testiranja gameplay-a u editoru, pokreni `Package_Windows.bat` za Win64 Shipping build.

## Originalni COD2 modeli i teksture

Javni repo ne sadrži originalne Call of Duty 2 IWD arhive niti izvučene vlasničke assete. Za lokalnu instalaciju koju poseduješ pripremljeni su ekstrakcioni alat i FBX importer. Pročitaj `Docs/ASSET_IMPORT_PIPELINE_SR.md`. Očekivani modeli su `SM_NationalAssembly`, `SM_PionirskiPark`, `SM_Presidency`, `SM_MainPostOfficePalace`, `SK_COD2_Soldier` i `SM_COD2_Weapon`, uz opcione `AN_COD2_Idle` / `AN_COD2_Walk` animacije. Datoteke moraš stvarno lokalno izvesti i uvesti pre nego što zamene proceduralne modele.

## Google Earth / Photorealistic 3D Tiles

Fotogrametrija uživo je opciona: zahteva Cesium for Unreal, omogućen Google Maps Platform Map Tiles API, projekat sa billing-om, ograničen API key i vidljivu atribuciju. Lokalno čuvaj ključ u `Config/Private/GoogleMapsApiKey.txt`; fajl je isključen iz Git-a. Pogledaj `Docs/GOOGLE_EARTH_CESIUM_SETUP_SR.md`. Offline mapa ostaje dostupna i bez Cesium-a/ključa.

## Arhiva starih demo verzija

- [Snapshot grana: archive/browser-render-demo-2026-10-29](https://github.com/milosgnjidicenquiries/cacilend-skupstina-fps/tree/archive/browser-render-demo-2026-10-29)
- [Uputstvo za arhivu](Archive/README_SR.md)
- [Browser demo na Renderu](https://cacilend-skupstina-browser.onrender.com)
- [Browser 3D galerija modela](https://cacilend-skupstina-browser.onrender.com/assets.html)

Folder `browser/` namerno ostaje u repo-u zato što ga Render objavljuje kao statički sajt. Ne premeštaj ga bez promene Render konfiguracije.

## Status validacije

GitHub Actions proverava JSON konfiguraciju, Python sintaksu, browser JS i osnovnu ravnotežu C++ zagrada. To **nije** C++ kompilacija niti gameplay test. Repo još nema verifikovan Windows EXE; UnrealBuildTool, cooking, pakovanje i testiranje moraju da se urade na Windows računaru sa instaliranim UE 5.8.