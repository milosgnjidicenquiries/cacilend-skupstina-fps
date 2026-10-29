"""Static contract tests for the Cacilend UE5 project; no Unreal installation required."""
from pathlib import Path
import json
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
FAILURES = []


def read(rel):
    path = ROOT / rel
    if not path.is_file():
        FAILURES.append(f"Missing required file: {rel}")
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def require(condition, message):
    if not condition:
        FAILURES.append(message)


def main():
    try:
        project = json.loads(read("CacilendUE.uproject"))
    except json.JSONDecodeError as exc:
        project = {}
        FAILURES.append(f"CacilendUE.uproject is invalid JSON: {exc}")

    require(project.get("EngineAssociation") == "5.8",
            "Project must target Unreal Engine 5.8.")
    require(any(module.get("Name") == "CacilendUE" for module in project.get("Modules", [])),
            "CacilendUE runtime module is missing from .uproject.")

    engine_ini = read("Config/DefaultEngine.ini")
    input_ini = read("Config/DefaultInput.ini")
    game_mode_h = read("Source/CacilendUE/CacilendGameMode.h")
    game_mode_cpp = read("Source/CacilendUE/CacilendGameMode.cpp")
    character_cpp = read("Source/CacilendUE/CacilendFPSCharacter.cpp")
    enemy_cpp = read("Source/CacilendUE/CacilendEnemy.cpp")
    hud_cpp = read("Source/CacilendUE/CacilendHUD.cpp")
    map_cpp = read("Source/CacilendUE/CacilendMapBuilder.cpp")
    setup_py = read("Content/Python/Setup_Cacilend.py")

    # Default class/map must agree with actual native C++ and editor bootstrap.
    require("GlobalDefaultGameMode=/Script/CacilendUE.CacilendGameMode" in engine_ini,
            "DefaultEngine.ini must name the CacilendGameMode class.")
    require("ACacilendFPSCharacter::StaticClass()" in game_mode_cpp,
            "GameMode must assign the custom FPS pawn.")
    require("ACacilendHUD::StaticClass()" in game_mode_cpp,
            "GameMode must assign the custom HUD.")
    require("ACacilendMapBuilder::StaticClass()" in game_mode_cpp,
            "GameMode must provide the runtime map-builder fallback.")
    require("GameDefaultMap=/Game/Maps/L_Cacilend" in engine_ini,
            "GameDefaultMap must point at /Game/Maps/L_Cacilend.")
    require("MAP_PATH = '/Game/Maps/L_Cacilend'" in setup_py,
            "Python setup and GameDefaultMap must target the same level.")
    require("CacilendMapBuilder" in setup_py and "PlayerStart" in setup_py,
            "Editor bootstrap must create the map builder and a PlayerStart.")

    # Legacy input mappings are still required by this C++ InputComponent setup.
    axis_bindings = set(re.findall(r'BindAxis\(TEXT\("([^"]+)"\)', character_cpp))
    action_bindings = set(re.findall(r'BindAction\(TEXT\("([^"]+)"', character_cpp))
    configured_axes = set(re.findall(r'\+AxisMappings=\(AxisName="([^"]+)"', input_ini))
    configured_actions = set(re.findall(r'\+ActionMappings=\(ActionName="([^"]+)"', input_ini))
    require(axis_bindings <= configured_axes,
            "Input axes missing from DefaultInput.ini: " + ", ".join(sorted(axis_bindings - configured_axes)))
    require(action_bindings <= configured_actions,
            "Input actions missing from DefaultInput.ini: " + ", ".join(sorted(action_bindings - configured_actions)))

    # HUD and combat features should remain connected to public character accessors.
    for accessor in ("GetHealth", "GetKills", "GetAmmo", "GetReserveAmmo", "IsReloading", "IsInInterior", "IsNearAssemblyEntrance"):
        require(accessor in hud_cpp or accessor in character_cpp,
                f"HUD/character contract missing accessor: {accessor}")
    require("MISIJA: OSIGURAJ PRILAZ I UDJI U ZGRADU" in hud_cpp and "SPACE SKOK" in hud_cpp,
            "HUD must show the mission objective and core movement/weapon controls.")
    require("GetWorldTimerManager().ClearTimer(ReloadTimerHandle)" in character_cpp and
            "bReloading = false;" in character_cpp and "bSprinting = false;" in character_cpp,
            "Player respawn must clear reload and sprint states.")
    require("FVector(22000.0f, -3300.0f, 100.0f)" in character_cpp and
            "FVector(22000.0f, -4600.0f, 100.0f)" in character_cpp,
            "Interior spawn must be separated from the E-to-exit trigger.")
    require("Enemy->TakeDamage(34.0f" in character_cpp and "AddKill()" in character_cpp,
            "Player firing path must damage enemies and track kills.")
    require("Player->TakeDamage(" in enemy_cpp,
            "Enemy attack path must be wired to player damage.")

    # A clean checkout may have no optional generated PBR files. Materials still
    # need a Tint parameter directly wired to BaseColor in that case.
    require("unreal.MaterialProperty.MP_BASE_COLOR" in setup_py and "else:" in setup_py and
            "connect_material_property(\n            tint, '', unreal.MaterialProperty.MP_BASE_COLOR" in setup_py,
            "Material generation must have a visible Tint-only fallback when textures are absent.")

    # UE discovery script must exist and each principal launcher must request the correct executable.
    helper = read("Scripts/Find_UE_5_8.ps1")
    require("Build.version" in helper and 'MajorVersion -ne 5' in helper and 'MinorVersion -ne 8' in helper,
            "UE discovery must verify the exact installed 5.8 engine version.")
    launchers = {
        "Build_Run_Cacilend_UE5.bat": "Build",
        "Setup_Cacilend_Assets.bat": "Editor",
        "Start_Cacilend_Editor.bat": "Editor",
        "Import_Cacilend_Assets.bat": "Editor",
        "Package_Windows.bat": "Package",
    }
    for rel, purpose in launchers.items():
        content = read(rel)
        require("Scripts\\Find_UE_5_8.ps1" in content and f"-Purpose {purpose}" in content,
                f"{rel} must use Find_UE_5_8.ps1 with Purpose {purpose}.")
    build_launcher = read("Build_Run_Cacilend_UE5.bat")
    package_launcher = read("Package_Windows.bat")
    require("Check_Cacilend_UE5_Requirements.ps1" in build_launcher,
            "Main build launcher must run the prerequisite checker automatically.")
    setup_launcher = read("Setup_Cacilend_Assets.bat")
    require("Wait_Cacilend_Setup.ps1" in setup_launcher and "UE5-setup-status.txt" in setup_launcher,
            "Setup launcher must wait for an explicit Editor setup completion marker.")
    require("write_setup_status('SUCCESS'" in setup_py and "write_setup_status('FAILED'" in setup_py,
            "Editor setup Python must report explicit success/failure status.")
    require((ROOT / "Scripts/Wait_Cacilend_Setup.ps1").is_file(),
            "Setup completion waiter is missing.")
    require("UE5-build.log" in build_launcher and "Tail 80" in build_launcher,
            "Main build launcher must retain build logs and show a useful error tail.")
    require("UE5-package.log" in package_launcher and "Tail 100" in package_launcher,
            "Packaging launcher must retain UAT logs and show a useful error tail.")
    require((ROOT / "Check_Cacilend_UE5_Requirements.bat").is_file(),
            "One-click UE5 requirements checker is missing.")
    require((ROOT / "Scripts/Check_Cacilend_UE5_Requirements.ps1").is_file(),
            "PowerShell preflight checker is missing.")

    if FAILURES:
        print("PROJECT CONTRACT CHECK FAILED")
        for failure in FAILURES:
            print(" - " + failure)
        return 1
    print("Project contract checks passed.")
    print(f"Input axes validated: {', '.join(sorted(axis_bindings))}")
    print(f"Input actions validated: {', '.join(sorted(action_bindings))}")
    print("GameMode, map, HUD/combat wiring, and UE5.8 launcher discovery validated.")
    print("Note: this is static validation; an actual UnrealBuildTool compile/gameplay test is still required.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
