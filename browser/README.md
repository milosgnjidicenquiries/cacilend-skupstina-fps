# Browser FPS practice demo

This is a browser-rendered companion to the Windows Unreal Engine source project. It uses Three.js (currently pinned to 0.186.1) with physically based materials, tone mapping, shadow maps, procedural surface textures, first-person movement, a rifle view model, HUD, robotic training targets, and a separate Assembly room.

It is not Unreal Engine running inside the browser, and it does not claim the fidelity of a finished AAA game. The 3D geometry is procedural and the visuals are an early prototype. Three.js is loaded from jsDelivr, so an internet connection is needed.

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
- Build command: echo "Static site - no build step"

Open the deployed URL in current Chrome or Edge with WebGL/hardware acceleration enabled.
