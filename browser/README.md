# Ćaćilend FPS — browser prototype

Browser-rendered companion to the Windows Unreal Engine source. Uses Three.js 0.186.1, procedural PBR-style materials, shadow maps, first-person controls, HUD, robotic training targets and a separate Assembly room.

## Map / Belgrade landmark block
The playable scene now includes an approximate, scaled layout of:
- Pionirski Park and its paths, fountain, benches and trees
- House of the National Assembly (Trg Nikole Pašića 13)
- Presidency / New Palace (Andrićev venac 1)
- Main Post Office Palace (Takovska 2)
- Surrounding streets and simplified urban blocks

This is a hand-built gameplay reconstruction guided by published location coordinates, not a georeferenced survey or copied Google Earth mesh. Use the **EARTH 3D** link in the game for the live Google Earth reference. Google Photorealistic 3D Tiles is not embedded: that requires a Google Maps Platform API key, enabled billing, a compliant 3D Tiles renderer and visible Google attribution.

## 3D models
The in-game model gallery is available at [assets.html](./assets.html). It displays 3D preview embeds for selected COD2 model sets and links to the creators' source pages. A CC0 rifle GLB is loaded at runtime from the public FPS Asset Kit repository; the procedural view model remains as fallback if the remote download fails.

The COD2-derived Sketchfab model binaries are **not copied into this repository/gameplay**. Although the selected pages indicate CC Attribution, they depict assets derived from a commercial game, so redistribution rights should be verified before embedding the files in a public build. For maps/buildings from a local COD2 installation, see the GPL-3.0 [cod2-to-glb converter](https://github.com/sniklaus/cod2-to-glb); it requires the user's own local `.iwd` game files.

See [ASSET_SOURCES.md](./ASSET_SOURCES.md) for source links and notes.

## Controls
- WASD: move
- Mouse: look
- Left mouse button: practice shot
- R: reload
- Shift: sprint
- E: enter/exit the Assembly room near its doorway
- Esc: release mouse / pause

## Render Static Site
- Repository: https://github.com/milosgnjidicenquiries/cacilend-skupstina-fps
- Branch: main
- Publish directory: browser
- Build command: `echo "Static site - no build step"`

Open the deployed URL in current Chrome or Edge with WebGL/hardware acceleration enabled. Three.js, the rifle asset and model previews need internet access.