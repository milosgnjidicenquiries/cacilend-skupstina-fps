# Ćacilend: Skupština FPS — Windows UE 5.8 prototip

Ovo je izvorni Unreal Engine projekat za Windows. Cilj je FPS prototip sa mapom oko Narodne skupštine i kampa Ćacilend, uz zasebnu unutrašnju scenu skupštinske sale. Nije gotov komercijalni naslov niti je grafika već na nivou najnovijeg Call of Duty-ja.

## Uključeno
- Unreal Engine 5.8 C++ projekat sa Windows/DX12 render podešavanjima.
- Proceduralna spoljašnja scena: prilazni put, fasada Skupštine, šatori, prepreke, palete, vozila i drveće.
- Odvojena unutrašnja sala, dostupna interakcijom na ulazu.
- First-person kamera, WASD/miš, skok, sprint, prototipno oružje, trzaj, municija, reload, health i HUD.
- Devet generičkih AI protivnika sa jednostavnim približavanjem i napadom.
- Skripte za generisanje PBR tekstura, editor asseta i pakovanje Windows builda.

## Važno ograničenje
Ovaj repo sadrži izvorni UE projekat, ne gotov EXE. Unreal Engine 5.8, Unreal Build Tool i Windows C++ build okruženje nisu dostupni u ovom okruženju, pa kompajliranje i testiranje moraju da se urade na Windows računaru.

Proceduralni modeli su prototipni. Za kvalitet na nivou velikog komercijalnog FPS-a potrebni su detaljni i pravilno licencirani modeli, materijali, skeletni karakteri i profesionalne animacije.

## Šta je potrebno
- Windows 10/11 64-bit
- Unreal Engine 5.8 preko Epic Games Launchera
- Visual Studio 2022: Desktop development with C++ i Game development with C++ workload-i, uz kompatibilan Windows SDK
- Za ozbiljniji rad preporuka je 32 GB RAM-a i GPU sa najmanje 8 GB VRAM-a.

## Prvo pokretanje
1. Preuzmi ili kloniraj repo u kratak folder, npr. C:\Games\CacilendUE.
2. Instaliraj UE 5.8 i Visual Studio 2022 C++ alate.
3. Pokreni Start_Cacilend_Editor.bat. Dozvoli Unreal-u da kompajlira C++ modul.
4. Pokreni Setup_Cacilend_Assets.bat da generiše i uveze teksture, materijale i mapu L_Cacilend.
5. Ako automatski Python setup ne uspe, u Unreal Editor-u izaberi Tools > Execute Python Script i pokreni Content/Python/Setup_Cacilend.py.
6. Otvori mapu L_Cacilend i pritisni Play.
7. Kada scena radi, pokreni Package_Windows.bat da pokušaš da napraviš Windows Shipping build.

## Kontrole
- WASD — kretanje
- Miš — pogled/nišanjenje
- Levi klik — pucanje
- R — reload
- Shift — sprint
- Space — skok
- E — ulaz/izlaz iz skupštinske sale
- Esc — prekida Play sesiju u editoru

## Struktura
- CacilendUE.uproject — Unreal projekat
- Source/CacilendUE — C++ karakter, AI, HUD i map builder
- Content/Python/Setup_Cacilend.py — kreira Unreal materijale i mapu
- Scripts/generate_textures.py — generiše PBR izvore
- Start_Cacilend_Editor.bat — otvara Unreal Editor
- Setup_Cacilend_Assets.bat — priprema teksture, assete i mapu
- Package_Windows.bat — Windows pakovanje

Ne distribuiraj build pre nego što se igra pokrene na čistom Windows računaru i ne prođu provere kontrola, kolizija, AI i prelaska između scena.
