# Cacilend: Skupstina FPS

Two related prototypes live in this repository:

- **Browser demo** in the browser/ directory: a Three.js first-person practice demo with PBR-style materials, ACES tone mapping, shadow maps, procedural textures, a HUD, robotic range targets, and an interior Assembly room. Deploy browser/ as a Render Static Site to play it in a web browser.
- **Windows source project** in the repository root: Unreal Engine 5.8 C++ project with a procedural scene and Windows packaging scripts.

The browser edition is inspired by modern UE-style rendering techniques; it is not Unreal Engine running in a browser and will not match a completed AAA Call of Duty title.

## Browser edition
See browser/README.md for controls and hosting. It requires a browser with WebGL and an internet connection to load Three.js from jsDelivr.

Render Static Site settings:
- Repository: https://github.com/milosgnjidicenquiries/cacilend-skupstina-fps
- Branch: main
- Publish directory: browser
- Build command: echo "Static site - no build step"

## Windows UE 5.8 build
Requirements: Unreal Engine 5.8, Visual Studio 2022 with C++ workloads, and a compatible Windows SDK.

1. Download/clone to a short Windows path such as C:\Games\CacilendUE.
2. Open CacilendUE.uproject and allow the C++ module to compile.
3. Run Setup_Cacilend_Assets.bat to generate PBR source textures and editor assets.
4. Open the generated map and press Play.
5. Run Package_Windows.bat to attempt a Win64 Shipping package.

This repo contains source code and scripts, not a verified packaged EXE. The Unreal project has not been compiled or run in this environment; the first Windows build needs validation.

Read README_SR.md for Serbian Windows instructions.
