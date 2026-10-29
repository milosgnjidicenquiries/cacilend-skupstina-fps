"""Extract relevant files from a LOCAL Call of Duty 2 installation.

All output goes under Build/LocalCOD2Extraction, which is git-ignored.
The script does not download COD2 files and never modifies the installed game.
"""
import argparse
import os
import zipfile
from pathlib import Path, PurePosixPath

PROJECT_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT = PROJECT_ROOT / "Build" / "LocalCOD2Extraction"
ALLOWED_PREFIXES = (
    "images/", "materials/", "maps/", "xanim/", "xmodel/",
    "xmodelalias/", "xmodelparts/", "xmodelsurfs/", "weapons/",
)
CANDIDATE_ROOTS = (
    Path(r"C:\\Program Files (x86)\\Activision\\Call of Duty 2"),
    Path(r"C:\\Program Files\\Activision\\Call of Duty 2"),
    Path(r"C:\\Games\\Call of Duty 2"),
    Path(r"D:\\Games\\Call of Duty 2"),
)


def game_main_dir(root_arg):
    candidates = []
    if root_arg:
        root = Path(root_arg).expanduser()
        candidates.extend([root / "main", root])
    env_root = os.environ.get("COD2_INSTALL_DIR")
    if env_root:
        candidates.extend([Path(env_root) / "main", Path(env_root)])
    for root in CANDIDATE_ROOTS:
        candidates.extend([root / "main", root])
    for candidate in candidates:
        if candidate.is_dir() and any(candidate.glob("*.iwd")):
            return candidate
    return None


def safe_output_path(destination, member_name):
    path = PurePosixPath(member_name.replace("\\", "/"))
    if path.is_absolute() or any(part in ("..", "") for part in path.parts):
        return None
    target = (destination / Path(*path.parts)).resolve()
    try:
        target.relative_to(destination.resolve())
    except ValueError:
        return None
    return target


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--game-root", help="COD2 installation root, or its main folder")
    parser.add_argument("--output", default=str(DEFAULT_OUTPUT), help="Local extraction directory (default: ignored Build folder)")
    args = parser.parse_args()

    main_dir = game_main_dir(args.game_root)
    if not main_dir:
        parser.error("Could not find *.iwd archives. Pass --game-root or set COD2_INSTALL_DIR.")

    output = Path(args.output).resolve()
    output.mkdir(parents=True, exist_ok=True)
    archives = sorted(main_dir.glob("*.iwd"))
    extracted = 0
    skipped = 0
    failed = 0

    print("COD2 install main folder: {}".format(main_dir))
    print("Local extraction output: {}".format(output))
    print("Archives discovered: {}".format(len(archives)))

    for archive in archives:
        try:
            with zipfile.ZipFile(str(archive), "r") as zf:
                archive_count = 0
                for info in zf.infolist():
                    if info.is_dir():
                        continue
                    normalized = info.filename.replace("\\", "/").lstrip("./").lower()
                    if not normalized.startswith(ALLOWED_PREFIXES):
                        skipped += 1
                        continue
                    target = safe_output_path(output, info.filename)
                    if target is None:
                        skipped += 1
                        continue
                    target.parent.mkdir(parents=True, exist_ok=True)
                    with zf.open(info, "r") as source, open(str(target), "wb") as dest:
                        while True:
                            chunk = source.read(1024 * 1024)
                            if not chunk:
                                break
                            dest.write(chunk)
                    extracted += 1
                    archive_count += 1
                print("[OK] {} -> {} relevant files".format(archive.name, archive_count))
        except (OSError, zipfile.BadZipFile, RuntimeError) as exc:
            failed += 1
            print("[WARN] Could not read {}: {}".format(archive.name, exc))

    print("")
    print("Extraction finished. Files written: {}; filtered files: {}; failed archives: {}.".format(extracted, skipped, failed))
    print("Next: install the COD2 Asset Importer Blender add-on and point it to this folder.")
    print("Do not copy this folder into a public Git repository or redistribute the extracted game files.")


if __name__ == "__main__":
    main()
