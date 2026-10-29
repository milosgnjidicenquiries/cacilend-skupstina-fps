LOCAL ASSET IMPORT FOLDER — NOT DISTRIBUTED BY GIT

Ovaj folder je namenjen modelima i teksturama koje ti lokalno uvezeš i imaš pravo da koristiš.
Nije mesto za javno kačenje originalnih COD2 arhiva (.iwd) ili drugih vlasničkih resursa.

Sačuvaj FBX fajlove i potrebne teksture u sledećim podfolderima:
Buildings/SM_NationalAssembly.fbx
Buildings/SM_PionirskiPark.fbx
Buildings/SM_Presidency.fbx
Buildings/SM_MainPostOfficePalace.fbx
Characters/SK_COD2_Soldier.fbx
Weapons/SM_COD2_Weapon.fbx
Animations/AN_COD2_Idle.fbx (opciono)
Animations/AN_COD2_Walk.fbx (opciono)

Za originalne COD2 modele iz svoje lokalne instalacije:
1. Pokreni Extract_COD2_Assets.bat [putanja_do_COD2_instalacije].
2. Pokreni `Install_COD2_Blender_Importer.bat`; dodatak https://github.com/mauserzjeh/cod-asset-importer uvozi COD2 D3DBSP mape i XModel-e, ne XAnim animacije.
3. U Blenderu uvezi željeni XModel, izvezi izabrani mesh/skeleton kao FBX i preimenuj ga tačno prema putanjama iznad.
4. Za animacije pokreni `Convert_COD2_Animations.bat` sa jednom lokalnom `.xanim_bin` datotekom, pa instaliraj `Install_COD2_Animation_Addon.bat`. XAnim import je eksperimentalan i može zahtevati ručno popravljanje/retarget.
5. Stavi teksture pored FBX-a i sačuvaj ih u odgovarajući podfolder ovog direktorijuma.
6. Pokreni `Import_Cacilend_Assets.bat` ili `Setup_Cacilend_Assets.bat`.

Ako mapa dolazi iz COD2 D3DBSP-a, ona nije geografski ispravna za centar Beograda. Koristi taj importer za zasebne modele/props, a stvarni raspored Beograda i Google 3D Tiles sloj drži nezavisno.
