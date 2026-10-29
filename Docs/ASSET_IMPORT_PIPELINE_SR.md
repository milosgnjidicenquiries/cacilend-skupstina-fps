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
3. Za privremeni CC0 placeholder oružja (nije COD2 original) možeš pokrenuti `Download_CC0_Weapon_Fallback.bat`; on ga smešta u `SourceArt/Import/Weapons/SM_CC0_Rifle.fbx`, koje se lokalno uvozi u UE5.
4. Ako koristiš svoju legalnu lokalnu instalaciju Call of Duty 2, pokreni `Extract_COD2_Assets.bat "C:\putanja\do\Call of Duty 2"`. Skripta čita lokalne `.iwd` ZIP arhive i kopira samo potrebne foldere u `Build/LocalCOD2Extraction`. Zatim automatski pravi `Build/LocalCOD2AssetCatalogue.md`, spisak mogućih modela, animacija, BSP mapa i tekstura. Te lokacije su ignorisane u Git-u i ne menjaju instalaciju igre.
5. Pokreni `Install_COD2_Blender_Importer.bat`. Skripta prvo preuzima gotov [COD Asset Importer v3.5 release ZIP](https://github.com/mauserzjeh/cod-asset-importer/releases/tag/v3.5), a ako to ne uspe pokušava izgradnju iz izvornog koda. Taj dodatak podržava COD2 D3DBSP mape i kompajlirane XModel-e, ne XAnim animacije. U Blenderu izaberi Edit → Preferences → Add-ons → Install from Disk, odaberi ZIP iz `Tools/third_party/cod-asset-importer/release`, pa omogući dodatak. Release v3.5 navodi Blender 4.3 podršku; preporučeno je da koristiš Blender 4.3 za taj paket. Dodatak zahteva da se sačuva originalna struktura foldera u `Build/LocalCOD2Extraction`.
6. U Blenderu, koristeći **CoD Asset Importer**, izaberi File → Import → CoD Asset Importer → Import model i odaberi željeni XModel. Ovaj dodatak može da uveze i COD2 D3DBSP mapu; međutim, COD2 mapa nije geografski Beograd. Za FPS prototip iz nje biraj pojedinačne vojnike, oružja i propove, a beogradski raspored ostavi kao poseban nivo. Izvezi odabrane mesh-eve kao FBX preko File → Export → FBX ili `Scripts/blender_export_selected_fbx.py`. Za karakter izaberi Skeletal Mesh i njegov armature; za statičnu pušku/prop dodaj `--static` pri korišćenju skripte.
7. **Animacije se obrađuju odvojeno od modela.** Pokreni `Convert_COD2_Animations.bat` i prevuci na njega jednu kompajliranu `.xanim_bin` datoteku iz `Build/LocalCOD2Extraction/xanim`. Skripta preuzima open-source [xmodelconverter v24.1](https://github.com/riicchhaarrd/xmodelconverter/releases/tag/v24.1) i pokušava da napravi `.xanim_export` u `Build/LocalCOD2Extraction/exported`. Zatim pokreni `Install_COD2_Animation_Addon.bat` i instaliraj `pv_blender_cod` dodatak iz [v0.9.0 release-a](https://github.com/prov3ntus/pv_blender_cod/releases/tag/v0.9.0). U Blenderu izaberi postojeći COD2 armature i pokušaj uvoz `.xanim_export`, pa izvezi animaciju kao FBX. **Ovo je eksperimentalni korak:** održavalac dodatka navodi poznate probleme sa XAnim importom; ako se animacija ne učita ili je deformisana, preskoči je za prvi build, koristi proxy/single-node fallback ili popravi/retargetuj animaciju ručno. Ne predstavljaj ovu konverziju kao proverenu dok se ne testira sa konkretnim XAnim fajlom.

   | Putanja | Namena |
   | --- | --- |
   | `Buildings/SM_NationalAssembly.fbx` | zamena fasade Skupštine |
   | `Buildings/SM_PionirskiPark.fbx` | opcioni statični park/prop |
   | `Buildings/SM_Presidency.fbx` | zamena zgrade Predsedništva |
   | `Buildings/SM_MainPostOfficePalace.fbx` | zamena Glavne pošte |
   | `Characters/SK_COD2_Soldier.fbx` | Skeletal Mesh vojnika |
   | `Weapons/SM_COD2_Weapon.fbx` | FPS Static Mesh COD2 puške |
   | `Weapons/SM_CC0_Rifle.fbx` | opciona CC0 fallback puška, nije COD2 original |
   | `Animations/AN_COD2_Idle.fbx` | opcioni idle, kompatibilan sa imported skeleton-om |
   | `Animations/AN_COD2_Walk.fbx` | opcioni walk, kompatibilan sa imported skeleton-om |

8. Sačuvaj izvezene FBX i teksture pod `SourceArt/Import/` koristeći tačno sledeće putanje i imena:

   | Putanja | Namena |
   | --- | --- |
   | `Buildings/SM_NationalAssembly.fbx` | opcioni model fasade Skupštine |
   | `Buildings/SM_PionirskiPark.fbx` | opcioni park/prop model |
   | `Buildings/SM_Presidency.fbx` | opcioni model Predsedništva/Novi dvor |
   | `Buildings/SM_MainPostOfficePalace.fbx` | opcioni model Glavne pošte |
   | `Characters/SK_COD2_Soldier.fbx` | skeletni model vojnika |
   | `Weapons/SM_COD2_Weapon.fbx` | static mesh COD2 puške |
   | `Weapons/SM_CC0_Rifle.fbx` | legalni privremeni fallback, nije COD2 original |
   | `Animations/AN_COD2_Idle.fbx` | opcioni idle FBX, isti skeleton |
   | `Animations/AN_COD2_Walk.fbx` | opcioni walk FBX, isti skeleton |

9. Sačuvaj teksture koje si legitimno izvezao pored odgovarajućih FBX fajlova. Koristi Blender FBX eksport koji kopira teksture ili spakuj teksture u folder uz FBX.
10. Pokreni `Import_Cacilend_Assets.bat` za sam import, ili `Setup_Cacilend_Assets.bat` za ceo setup. U logu potraži `[Cacilend Asset Import] Imported ...` i proveri da je uvezen svaki očekivani asset.
11. U Unreal-u otvori `/Game/Maps/L_Cacilend`, pritisni **Play** i proveri orijentaciju modela, veličinu, collision, skeleton, animacije i materijale.
12. Tek kad je testiranje uspešno, pokreni `Package_Windows.bat` da napraviš Windows Shipping build.

## Važno za COD2 modele

Ovaj repozitorijum ne sadrži i ne preuzima originalne Activision/Call of Duty 2 `.iwd` arhive ili izvučene proprietary modele. Import pipeline je spreman da koristi tvoje lokalno obezbeđene datoteke, ali same datoteke treba da uvezeš. Nemoj ih commit-ovati u javni GitHub repozitorijum niti javno distribuirati bez odgovarajućih prava.

Kada se izveze samo COD2 mapa, ona geografski ne predstavlja Beograd. Koristi COD2 XModel/XAnim za karaktere, oružje i zasebne propove; zadržavaj koordinatni raspored Beograda kao odvojeni nivo.

## Google 3D tiles

Pogledaj [Google Earth / Cesium setup](./GOOGLE_EARTH_CESIUM_SETUP_SR.md). Google-ov 3D tiles sloj se strimuje uživo i nije kopiran u Git. Za live fotogrametriju traže se Cesium for Unreal, Google Maps Platform API key, omogućen Map Tiles API, billing i prikaz atribucije. Ako se te stavke ne podese, offline UE mapa ostaje dostupna.
