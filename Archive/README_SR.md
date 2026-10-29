# Arhiva prethodnih demo verzija

Primarni proizvod ovog repozitorijuma je Windows FPS projekat **Unreal Engine 5.8** u korenu repozitorijuma. Ovaj direktorijum evidentira prethodne prototipe tako da se razvoj ne pomeša sa browser demo-om.

## Sačuvani snapshot

- **Browser / Render demo pre UE5 nadogradnje:** branch [archive/browser-render-demo-2026-10-29](https://github.com/milosgnjidicenquiries/cacilend-skupstina-fps/tree/archive/browser-render-demo-2026-10-29)
- **Tačan commit snapshot-a:** 5411f165908c2ac274e4a8309a17b432901a710e
- **Javno pokrenuta browser verzija:** https://cacilend-skupstina-browser.onrender.com
- **COD2 model galerija (browser):** https://cacilend-skupstina-browser.onrender.com/assets.html

Browser kod ostaje u browser/ zato što Render Static Site koristi taj direktorijum kao publish root. Ne brišite ga i ne premeštajte bez promene Render podešavanja. Glavni zadatak i autoritativan projekat za nastavak razvoja je CacilendUE.uproject plus Source/, Config/, Content/Python/ i Windows build skripte.

## Zašto su modeli originalne igre odvojeni

Arhiva čuva kod i linkove demonstracija, a ne kopije komercijalnih Call of Duty 2 paketa. Originalne .iwd datoteke, izvučeni teksture/meshevi i drugi vlasnički resursi ne treba da se komituju u javni Git repozitorijum. Uvezene lokalne resurse držite u SourceArt/Import/ i proverite pravo korišćenja pre deljenja projekta ili builda.
