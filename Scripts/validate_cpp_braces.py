"""Static structural checks for C++ source; not a substitute for UnrealBuildTool."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "Source"
LITERAL_OR_COMMENT = re.compile(
    r"""//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'""",
    re.DOTALL,
)
PAIRS = {")": "(", "}": "{", "]": "["}
OPEN = set(PAIRS.values())


def check_file(path):
    source = path.read_text(encoding="utf-8", errors="replace")
    clean = LITERAL_OR_COMMENT.sub(lambda match: "\n" * match.group(0).count("\n"), source)
    stack = []
    line = 1
    for char in clean:
        if char == "\n":
            line += 1
        elif char in OPEN:
            stack.append((char, line))
        elif char in PAIRS:
            if not stack or stack[-1][0] != PAIRS[char]:
                raise ValueError("{}:{} unexpected {}".format(path.relative_to(ROOT), line, char))
            stack.pop()
    if stack:
        char, opening_line = stack[-1]
        raise ValueError("{}:{} unclosed {}".format(path.relative_to(ROOT), opening_line, char))


def require_text(path, fragment):
    if not path.is_file():
        raise ValueError("Required file missing: {}".format(path.relative_to(ROOT)))
    if fragment not in path.read_text(encoding="utf-8", errors="replace"):
        raise ValueError("{}: required C++ symbol/text not found: {}".format(path.relative_to(ROOT), fragment))


def check_project_contract():
    map_h = SOURCE / "CacilendUE" / "CacilendMapBuilder.h"
    map_cpp = SOURCE / "CacilendUE" / "CacilendMapBuilder.cpp"
    char_h = SOURCE / "CacilendUE" / "CacilendFPSCharacter.h"
    char_cpp = SOURCE / "CacilendUE" / "CacilendFPSCharacter.cpp"
    enemy_h = SOURCE / "CacilendUE" / "CacilendEnemy.h"
    enemy_cpp = SOURCE / "CacilendUE" / "CacilendEnemy.cpp"
    pairs = [
        (map_h, "UStaticMeshComponent* AddImportedMesh("),
        (map_cpp, "ACacilendMapBuilder::AddImportedMesh("),
        (map_h, "void BuildPionirskiPark();"),
        (map_cpp, "void ACacilendMapBuilder::BuildPionirskiPark()"),
        (map_h, "void BuildPresidency();"),
        (map_cpp, "void ACacilendMapBuilder::BuildPresidency()"),
        (map_h, "void BuildMainPostOffice();"),
        (map_cpp, "void ACacilendMapBuilder::BuildMainPostOffice()"),
        (map_h, "void BuildSurroundingBuildings();"),
        (map_cpp, "void ACacilendMapBuilder::BuildSurroundingBuildings()"),
        (map_cpp, 'FParse::Param(FCommandLine::Get(), TEXT("CacilendUseGooglePhotogrammetry"))'),
        (char_h, "void SetPhotogrammetryVisible(bool bVisible);"),
        (char_cpp, "void ACacilendFPSCharacter::SetPhotogrammetryVisible(bool bVisible)"),
        (char_cpp, 'if (!FParse::Param(FCommandLine::Get(), TEXT("CacilendUseGooglePhotogrammetry")) || !GetWorld())'),
        (char_cpp, "SM_COD2_Weapon.SM_COD2_Weapon"),
        (char_cpp, "SM_CC0_Rifle.SM_CC0_Rifle"),
        (char_cpp, "const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);"),
        (enemy_h, "CurrentImportedAnimation"),
        (enemy_cpp, "SK_COD2_Soldier.SK_COD2_Soldier"),
    ]
    for path, fragment in pairs:
        require_text(path, fragment)
    if "GetAnimation()" in enemy_cpp.read_text(encoding="utf-8", errors="replace"):
        raise ValueError("CacilendEnemy.cpp still uses the unverified GetAnimation() getter; use tracked animation state.")
    char_source = char_cpp.read_text(encoding="utf-8", errors="replace")
    if char_source.count("const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);") != 2:
        raise ValueError("MoveForward and MoveRight must both use yaw-only movement vectors.")
    if "FRotationMatrix(Controller->GetControlRotation()).GetUnitAxis" in char_source:
        raise ValueError("Movement still depends on camera pitch; use yaw-only movement vectors.")


def main():
    source_files = sorted(list(SOURCE.rglob("*.cpp")) + list(SOURCE.rglob("*.h")))
    if not source_files:
        raise SystemExit("No C++ source files were found.")
    failures = []
    for path in source_files:
        try:
            check_file(path)
        except ValueError as exc:
            failures.append(str(exc))
    try:
        check_project_contract()
    except ValueError as exc:
        failures.append(str(exc))
    if failures:
        print("\n".join(failures))
        return 1
    print("C++ delimiter and required-symbol checks passed for {} source/header files.".format(len(source_files)))
    print("Note: an actual Unreal Engine 5.8 C++ build is still required.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
