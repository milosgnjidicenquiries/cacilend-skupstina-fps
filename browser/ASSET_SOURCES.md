# 3D asset sources and integration status

This file records the models used or reviewed for the Ćaćilend FPS prototype.

## COD2 character models

- [British Infantry — Call of Duty 2 HD](https://sketchfab.com/3d-models/british-infantry-call-of-duty-2-hd-15e7173ec5a54414a01d76f18a7cdc3d)
- [Normandy Nazis — Call of Duty 2 HD](https://sketchfab.com/3d-models/normandy-nazis-call-of-duty-2-hd-9898d3b4ba89452a9acaf15345e2c15f)
- [USSR / Nazis — Call of Duty 2 HD](https://sketchfab.com/3d-models/ussr-nazis-call-of-duty-2-hd-46a1f1af8f864bc2b423b1302b1c1b30)

## COD2 weapons

- [SMGs — Call of Duty 2 HD](https://sketchfab.com/3d-models/smgs-call-of-duty-2-hd-fcf4434b34fd496fb6f89f0a3380904f)
- [Semi-Auto & Bolt Action — Call of Duty 2 HD](https://sketchfab.com/3d-models/semi-auto-bolt-action-call-of-duty-2-hd-a40e0145ca1f4c8eb31dfac595eefb8c)
- [Handguns — Call of Duty 2 HD](https://sketchfab.com/3d-models/handguns-call-of-duty-2-hd-1b366e61e20f48368528cf52d1bdb215)

The source pages currently present these models as downloadable and display a Creative Commons Attribution label. That label alone does not establish that the uploader had rights to relicense proprietary Call of Duty 2 game assets. The models are therefore linked in the online gallery but their binary files have not been repackaged into the public game build.

## Optional CC0 weapon fallback for the UE5 project

- [FPS Asset Kit](https://github.com/petroulacl/fps-asset-kit) describes the pack as CC0/public domain and attributes the Flat Guns models to OpenGameArt.
- Run `Download_CC0_Weapon_Fallback.bat` to fetch the FBX for local import as `SM_CC0_Rifle.fbx`.
- It is only a temporary model; it does not replace the COD2 original. The UE5 character prefers `SM_COD2_Weapon` when it exists, then tries `SM_CC0_Rifle`, then falls back to procedural geometry.

## Weapon GLB currently loaded at runtime

- [FPS Asset Kit (CC0)](https://github.com/petroulacl/fps-asset-kit)
- File: weapons/flat_guns_west/Flat Guns West/GLB/Rifle_Assault_West.glb
- Runtime URL: https://raw.githubusercontent.com/petroulacl/fps-asset-kit/main/weapons/flat_guns_west/Flat%20Guns%20West/GLB/Rifle_Assault_West.glb

This is a lawful placeholder weapon model, not an original COD2 model. If the external GLB is unavailable, the game keeps its built-in procedural rifle.

## COD2 map/building extraction

- [cod2-to-glb](https://github.com/sniklaus/cod2-to-glb) — GPL-3.0 conversion utility; it imports map data from the user's own `.iwd` files and exports GLB.
- [Blender-CoD](https://github.com/CoDEmanX/blender-cod) — Blender tools for Call of Duty model/animation workflows.

No Call of Duty game archives or raw proprietary map/model files are committed here. The game includes a hand-built, approximate reconstruction of the Belgrade area instead.

## Belgrade area references

- [Pionirski Park coordinates / context](https://en.wikipedia.org/wiki/Pioneers_Park,_Belgrade)
- [House of the National Assembly coordinates / context](https://en.wikipedia.org/wiki/House_of_the_National_Assembly,_Belgrade)
- [Presidency address: Andrićev venac 1](https://www.predsednik.rs/en/contact/address-book)
- [Main Post Office Palace: Takovska 2, coordinates](https://en.wikipedia.org/wiki/Main_Post_Office_Palace,_Belgrade)
- [Official Assembly map of government buildings](https://www.parlament.gov.rs/upload/images/large/mapa_skupstina_eng.jpg)
- [Google Earth live reference](https://earth.google.com/web/@44.8106,20.4652,700a,35y,0h,0t,0r)

## Google Earth 3D integration note

The game opens Google Earth in a separate tab for live visual comparison. Actual photorealistic 3D Tiles inside the Three.js level need a Google Maps Platform project, a restricted API key, billing enabled, a compatible 3D Tiles renderer and required attribution. Google documents the requirements at [Map Tiles API overview](https://developers.google.com/maps/documentation/tile/overview) and [usage and billing](https://developers.google.com/maps/documentation/tile/usage-and-billing). No Google API key is embedded in the public code.