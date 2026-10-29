"""Create a LOCAL catalogue for assets extracted from a user-owned COD2 install.

Writes Build/LocalCOD2AssetCatalogue.md; Build/ and extracted game files are git-ignored.
No extracted file data is uploaded or copied into public source paths.
"""
import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SOURCE = ROOT / "Build" / "LocalCOD2Extraction"
DEFAULT_REPORT = ROOT / "Build" / "LocalCOD2AssetCatalogue.md"
MODEL_FOLDERS = ("xmodel", "xmodelalias", "xmodelparts", "xmodelsurfs")
ANIMATION_FOLDERS = ("xanim",)
MAP_FOLDERS = ("maps",)
TEXTURE_FOLDERS = ("images", "materials")
FILTER_TERMS = ("weapon", "gun", "rifle", "pistol", "mp40", "thompson", "ppsh", "kar98", "mosin", "soldier", "german", "russian", "american", "british", "truck", "car", "crate", "barrel", "tent", "building", "house", "wall", "door", "window", "tree", "fence")

def file_list(base, folders):
    found = []
    for folder in folders:
        path = base / folder
        if path.is_dir():
            found.extend(p for p in path.rglob("*") if p.is_file())
    return sorted(found, key=lambda p: str(p).lower())


def md_escape(text):
    return str(text).replace("|", "\\|").replace("\\r", " ").replace("\\n", " ")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", default=str(DEFAULT_SOURCE))
    parser.add_argument("--output", default=str(DEFAULT_REPORT))
    args = parser.parse_args()
    source = Path(args.source).resolve()
    output = Path(args.output).resolve()
    if not source.is_dir():
        parser.error("Extraction folder not found: {}. Run Extract_COD2_Assets.bat first.".format(source))

    models = file_list(source, MODEL_FOLDERS)
    animations = file_list(source, ANIMATION_FOLDERS)
    maps = file_list(source, MAP_FOLDERS)
    textures = file_list(source, TEXTURE_FOLDERS)
    all_candidates = models + animations
    recommended = [p for p in all_candidates if any(term in p.stem.lower() for term in FILTER_TERMS)]

    lines = [
        "# Local Call of Duty 2 asset catalogue",
        "",
        "This report is generated from a local installation and remains in the git-ignored Build folder.",
        "",
        "- Extraction root: {}".format(md_escape(source)),
        "- XModel-related files: {}".format(len(models)),
        "- XAnim files: {}".format(len(animations)),
        "- BSP/map files: {}".format(len(maps)),
        "- Image/material files: {}".format(len(textures)),
        "",
        "## Suggested XModel/XAnim candidates",
        "",
        "Filenames are search hints only. Check the actual model, skeleton and material dependencies in Blender.",
        "",
    ]
    if recommended:
        for path in recommended:
            rel = path.relative_to(source).as_posix()
            group = "animation" if path in animations else "model/mesh"
            lines.append("- [{}] {}".format(group, md_escape(rel)))
    else:
        lines.append("No names matched the heuristic. Browse all XModel/XAnim files in the extraction folder.")
    lines.extend(["", "## Maps / BSP files", ""])
    if maps:
        lines.extend("- {}".format(md_escape(p.relative_to(source).as_posix())) for p in maps[:1000])
    else:
        lines.append("No map files found.")
    lines.extend(["", "## Notes", ""])
    lines.append("- Use XModel/XAnim for characters, weapons and standalone props; COD2 map geography is not a Belgrade map.")
    lines.append("- Do not upload extracted files or this catalogue to a public distribution channel.")
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text("\\n".join(lines) + "\\n", encoding="utf-8")
    print("Wrote local asset catalogue: {}".format(output))
    print("Counts: models={}, animations={}, maps={}, images/materials={}".format(len(models), len(animations), len(maps), len(textures)))


if __name__ == "__main__":
    main()
