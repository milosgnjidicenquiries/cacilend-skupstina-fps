# Cacilend: Skupstina FPS

Windows-first Unreal Engine 5.8 C++ FPS prototype inspired by the user's visual reference. The project is a work in progress, not a finished Call of Duty-level game.

## Requirements
- Unreal Engine 5.8
- Visual Studio 2022 with Desktop development with C++ and Game development with C++ workloads
- Windows SDK supported by the installed Unreal version

## Build
1. Clone/download this repository to a short local path such as `C:\\Games\\CacilendUE`.
2. Right-click `CacilendUE.uproject` and generate Visual Studio project files if prompted.
3. Open the project in Unreal Editor and allow the C++ modules to compile.
4. Run `Setup_Cacilend_Assets.bat` to generate source PBR textures and editor-generated level assets.
5. Reopen the editor, open the generated map, and press Play.
6. Use `Package_Windows.bat` after the project builds successfully to package a Windows build.

## Controls (prototype)
WASD move, mouse look, left mouse fire, R reload, Shift sprint, Space jump, E interact/enter-exit, Esc pause.

## Notes
- `Content/Python/Setup_Cacilend.py` creates prototype geometry and maps inside the Unreal Editor.
- `Scripts/generate_textures.py` generates texture images at setup time; generated texture outputs are intentionally not committed.
- This repository contains source files and setup scripts, not a prebuilt `.exe` or a verified packaged build.
- Replace prototype geometry with properly licensed detailed assets and perform an Unreal Editor validation pass before treating the game as release-ready.

Read `README_SR.md` for Serbian setup instructions.
