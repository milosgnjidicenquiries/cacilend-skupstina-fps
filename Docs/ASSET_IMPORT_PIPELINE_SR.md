# UE5 asset pipeline — original model import (Serbian)

## Šta je primarni projekat

Primarni projekat je **CacilendUE.uproject — Unreal Engine 5.8 / Windows**. Browser/Render demo je sačuvan u grani [archive/browser-render-demo-2026-10-29](https://github.com/milosgnjidicenquiries/cacilend-skupstina-fps/tree/archive/browser-render-demo-2026-10-29) i u folderu `browser/` zato što Render statički sajt koristi taj folder.

## Šta je već pripremljeno u UE5 kodu

- Proceduralni offline fallback za skupštinski prilaz, Pionirski park, Predsedništvo, Glavnu poštu, okolne blokove, šatore i prepreke.
- `AddImportedMesh()` omogućava da uvezeni Static Mesh zameni proceduralnu fasadu/park kad model postoji.
- AI protivnik učitava Skeletal Mesh na putanji `/Game/Cacilend/Imported/Characters/SK_COD2_Soldier` ako je uvezen; u suprotnom ostaje proxy vojnik.
- FPS oružje učitava `/Game/Cacilend/Imported/Weapons/SM_COD2_Weapon` ako postoji; u suprotnom koristi proceduralni model.
- AI može da pusti opcione animacije `AN_COD2_Idle` i `AN_COD2_Walk` ako ih uvezeš uz isti skeleton.
- `Setup_Cacilend_Assets.bat` pokreće generisanje tekstura, import modela, kreiranje materijala i mape, pa pokušava opcioni Cesium setup.

## Uvoz modela i tekstura

1. Preuzmi/kloniraj repozitorijum na kratak Windows put, npr. `C:\Games\CacilendUE`.
2. Uveri se da su instalirani Unreal Engine 5.8 i Visual Studio 2022 C++ workloads.
3. Ako koristiš svoju legalnu lokalnu instalaciju Call of Duty 2, pokreni `Extract_COD2_Assets.bat "C:\putanja\do\Call of Duty 2"`. Skripta čita lokalne `.iwd` ZIP arhive i kopira samo potrebne foldere u `Build/LocalCOD2Extraction`. Ta lokacija je ignorisana u Git-u i ne menja instalaciju igre.
4. Instaliraj Blender dodatak [Call of Duty Asset Importer](https://github.com/mauserzjeh/cod-asset-importer), koji podržava COD2 D3DBSP mape i XModel formate. Podesi addon da čita `Build/LocalCOD2Extraction` — mora da ostane očuvana originalna struktura foldera.
5. U Blenderu uvezi željeni XModel/XAnim. Izvezi odabrani objekat kao FBX preko File → Export → FBX ili pomoću `Scripts/blender_export_selected_fbx.py`. Za Skeletal Mesh izaberi mesh i njegov armature; za statičnu pušku/zgradu dodaj `--static` kad koristiš skriptu.
6. Sačuvaj FBX i teksture pod `SourceArt/Import/` koristeći tačno sledeće putanje i imena:

   | Putanja | Namena |
   | --- | --- |
   | `Buildings/SM_NationalAssembly.fbx` | zamena fasade Skupštine |
   | `Buildings/SM_PionirskiPark.fbx` | opcioni statični park/prop |
   | `Buildings/SM_Presidency.fbx` | zamena zgrade Predsedništva |
   | `Buildings/SM_MainPostOfficePalace.fbx` | zamena Glavne pošte |
   | `Characters/SK_COD2_Soldier.fbx` | Skeletal Mesh vojnika |
   | `Weapons/SM_COD2_Weapon.fbx` | FPS Static Mesh puške |
   | `Animations/AN_COD2_Idle.fbx` | opcioni idle, kompatibilan sa imported skeleton-om |
   | `Animations/AN_COD2_Walk.fbx` | opcioni walk, kompatibilan sa imported skeleton-om |

7. Sačuvaj teksture koje si legitimno izvezao pored odgovarajućih FBX fajlova. Koristi Blender FBX eksport koji kopira teksture ili spakuj teksture u folder uz FBX.
8. Pokreni `Import_Cacilend_Assets.bat` za sam import, ili `Setup_Cacilend_Assets.bat` za ceo setup.
9. U Unreal-u otvori mapu `/Game/Maps/L_Cacilend`, pritisni **Play** i proveri orijentaciju modela, veličinu, collision i materijale.
10. Kad je testiranje uspešno, pokreni `Package_Windows.bat` da napraviš Windows Shipping build.

## Važno za COD2 modele

Ovaj repozitorijum ne sadrži i ne preuzima originalne Activision/Call of Duty 2 `.iwd` arhive ili izvučene proprietary modele. Import pipeline je spreman da koristi tvoje lokalno obezbeđene datoteke, ali same datoteke treba da uvezeš. Nemoj ih commit-ovati u javni GitHub repozitorijum niti javno distribuirati bez odgovarajućih prava.

Kada se izveze samo COD2 mapa, ona geografski ne predstavlja Beograd. Koristi COD2 XModel/XAnim za karaktere, oružje i zasebne propove; zadržavaj koordinatni raspored Beograda kao odvojeni nivo.

## Google 3D tiles

Pogledaj [Google Earth / Cesium setup](./GOOGLE_EARTH_CESIUM_SETUP_SR.md). Google-ov 3D tiles sloj se strimuje uživo i nije kopiran u Git. Za live fotogrametriju traže se Cesium for Unreal, Google Maps Platform API key, omogućen Map Tiles API, billing i prikaz atribucije. Ako se te stavke ne podese, offline UE mapa ostaje dostupna.
