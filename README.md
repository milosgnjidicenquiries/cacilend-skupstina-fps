# Cacilend: Skupstina FPS — Unreal Engine 5.8

**Primary project: Windows Unreal Engine 5.8 C++ FPS.** The browser/Render version is a legacy demo kept for reference and compatibility with the existing Render Static Site.

## Primary product: UE5 Windows project

- `CacilendUE.uproject` — Unreal Engine 5.8 project file.
- `Source/CacilendUE/` — FPS character, weapon handling, enemies and procedural scene builder.
- `Content/Python/Setup_Cacilend.py` — creates the playable level, material library and imports locally supplied assets.
- `Content/Python/Import_Cacilend_Assets.py` — FBX, skeletal mesh, animation and texture import pipeline.
- `Content/Python/Setup_Google3DTiles_Cesium.py` — optional live Google Photorealistic 3D Tiles setup through Cesium for Unreal.
- `Scripts/generate_textures.py` — generates local PBR texture sources.
- `Scripts/extract_cod2_assets.py` / `Extract_COD2_Assets.bat` — local-only extraction of relevant `.iwd` contents from a COD2 installation.
- `Build_Run_Cacilend_UE5.bat` — attempts a real UE5.8 C++ editor build, then launches the editor asset/map setup.
- `Import_Cacilend_Assets.bat`, `Setup_Cacilend_Assets.bat`, `Package_Windows.bat` — Windows tools.
- `Install_COD2_Blender_Importer.bat`, `Extract_COD2_Assets.bat` and `Download_CC0_Weapon_Fallback.bat` — local asset preparation.

The UE5 scene includes offline fallback geometry for the Assembly, Pionirski Park, Presidency/New Palace, Main Post Office Palace, surrounding urban blocks and the encampment. When matching imported assets exist at the documented `/Game/Cacilend/Imported/...` paths, the map builder/character/weapon code uses them. Without local assets, the project remains a procedural prototype.

## Quick start on Windows

Requirements: Windows 10/11 64-bit, Unreal Engine 5.8 and Visual Studio 2022 with Desktop development with C++ / Game development with C++ workloads and a compatible Windows SDK.

1. Clone/download this repository to a short path such as `C:\\Games\\CacilendUE`.
2. Open `CacilendUE.uproject` and let Unreal/Visual Studio compile the C++ module.
3. Run `Build_Run_Cacilend_UE5.bat` to invoke UnrealBuildTool for `CacilendUEEditor Win64 Development` and then launch setup in Unreal Editor.
4. (Optional) If using a locally installed COD2 copy, read `Docs/ASSET_IMPORT_PIPELINE_SR.md`, run `Extract_COD2_Assets.bat`, install the Blender importer via `Install_COD2_Blender_Importer.bat`, export selected models to FBX, and place them under `SourceArt/Import/`.
5. Run `Setup_Cacilend_Assets.bat` when you want to rerun generation/import without rebuilding C++.
6. In the Editor, open `/Game/Maps/L_Cacilend` and press **Play**.
7. After Editor gameplay is validated, run `Package_Windows.bat` to attempt a Win64 Shipping package.

## Original COD2 models and textures

No proprietary Call of Duty 2 IWD archives or extracted game assets are bundled in this public repository. For a user-owned local installation, the project includes an extraction helper and an FBX import pipeline. See `Docs/ASSET_IMPORT_PIPELINE_SR.md`. The model importer targets are `SM_NationalAssembly`, `SM_PionirskiPark`, `SM_Presidency`, `SM_MainPostOfficePalace`, `SK_COD2_Soldier`, and `SM_COD2_Weapon`, with optional `AN_COD2_Idle` / `AN_COD2_Walk` animations. These assets must actually be supplied/imported locally before they replace procedural fallbacks.

## Google Earth / Photorealistic 3D Tiles

Live Google photogrammetry is optional. It requires the Cesium for Unreal plugin, an enabled Google Maps Platform Map Tiles API, a billing-enabled project, a restricted API key and visible attribution. Store the key locally in `Config/Private/GoogleMapsApiKey.txt`; it is git-ignored. Read `Docs/GOOGLE_EARTH_CESIUM_SETUP_SR.md`. The offline map stays usable without Cesium or an API key.

## Archive of previous demos

- [Snapshot branch: archive/browser-render-demo-2026-10-29](https://github.com/milosgnjidicenquiries/cacilend-skupstina-fps/tree/archive/browser-render-demo-2026-10-29)
- [Archive notes](Archive/README_SR.md)
- [Browser demo on Render](https://cacilend-skupstina-browser.onrender.com)
- [Browser demo's 3D model catalogue](https://cacilend-skupstina-browser.onrender.com/assets.html)

The `browser/` folder is intentionally retained because Render publishes that folder. Do not remove or move it unless the Render service configuration is changed.

## Validation status

GitHub Actions validates project JSON, Python syntax, browser JS and basic C++ delimiter balance. This is **not** a C++ compile or gameplay test. The repository does not yet contain a verified Windows EXE; UnrealBuildTool, cooking, packaging and gameplay must be tested on a Windows computer with UE 5.8 installed.

See `README_SR.md` for the Serbian instructions.