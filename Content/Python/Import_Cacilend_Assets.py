"""Import locally supplied, legally usable FBX/texture assets into the UE5 project.

Expected folders under SourceArt/Import:
  Buildings/SM_NationalAssembly.fbx
  Buildings/SM_PionirskiPark.fbx
  Buildings/SM_Presidency.fbx
  Buildings/SM_MainPostOfficePalace.fbx
  Characters/SK_COD2_Soldier.fbx
  Weapons/SM_COD2_Weapon.fbx
  Animations/AN_COD2_Idle.fbx (optional; must match the imported skeleton)
  Animations/AN_COD2_Walk.fbx (optional; must match the imported skeleton)

Extracted proprietary game assets are intentionally not included in the public repository.
Run from Unreal Editor with Tools > Execute Python Script, or via the supplied BAT.
"""
import os
import unreal

ROOT = str(unreal.Paths.project_dir()).rstrip("/\\")
SOURCE_ROOT = os.path.join(ROOT, "SourceArt", "Import")
DEST_ROOT = "/Game/Cacilend/Imported"
SUPPORTED_FBX = {".fbx"}
SUPPORTED_TEXTURES = {".png", ".jpg", ".jpeg", ".tga", ".bmp", ".tif", ".tiff"}
IGNORED_EXTENSIONS = {".glb", ".gltf", ".obj", ".blend", ".xmodel", ".xanim", ".iwd", ".zip"}


def log(message):
    unreal.log("[Cacilend Asset Import] " + str(message))


def set_prop(obj, name, value):
    try:
        obj.set_editor_property(name, value)
        return True
    except Exception as exc:
        log("Property skipped: {} ({})".format(name, exc))
        return False


def ensure_destination(path):
    unreal.EditorAssetLibrary.make_directory(path)


def category_for(relative_dir):
    parts = [p for p in relative_dir.replace("\\", "/").split("/") if p]
    if not parts:
        return "Misc"
    first = parts[0].lower()
    mapping = {
        "building": "Buildings",
        "buildings": "Buildings",
        "character": "Characters",
        "characters": "Characters",
        "soldiers": "Characters",
        "weapon": "Weapons",
        "weapons": "Weapons",
        "animation": "Animations",
        "animations": "Animations",
        "maps": "Maps",
        "map": "Maps",
        "cod2maps": "COD2Maps",
        "textures": "Textures",
        "materials": "Materials",
        "vehicles": "Vehicles",
        "props": "Props",
    }
    return mapping.get(first, parts[0])


def make_fbx_options(filename, category, skeleton=None):
    options = unreal.FbxImportUI()
    is_animation = category == "Animations" or os.path.basename(filename).lower().startswith("an_")
    is_skeletal = category == "Characters" or os.path.basename(filename).lower().startswith("sk_")
    set_prop(options, "import_mesh", not is_animation)
    set_prop(options, "import_materials", True)
    set_prop(options, "import_textures", True)
    set_prop(options, "import_animations", is_animation)
    set_prop(options, "import_as_skeletal", is_skeletal or is_animation)

    import_type = getattr(unreal, "FBXImportType", None)
    if import_type:
        enum_name = "FBXIT_SKELETAL_MESH" if (is_skeletal or is_animation) else "FBXIT_STATIC_MESH"
        enum_value = getattr(import_type, enum_name, None)
        if enum_value is not None:
            set_prop(options, "mesh_type_to_import", enum_value)

    if is_skeletal:
        set_prop(options, "create_physics_asset", False)
        set_prop(options, "import_animations", False)
    if is_animation:
        set_prop(options, "import_mesh", False)
        set_prop(options, "import_animations", True)
        if skeleton:
            set_prop(options, "skeleton", skeleton)
    return options


def make_task(filename, destination, category, skeleton=None):
    task = unreal.AssetImportTask()
    set_prop(task, "filename", filename)
    set_prop(task, "destination_path", destination)
    set_prop(task, "automated", True)
    set_prop(task, "replace_existing", True)
    set_prop(task, "save", True)
    ext = os.path.splitext(filename)[1].lower()
    if ext in SUPPORTED_FBX:
        set_prop(task, "options", make_fbx_options(filename, category, skeleton))
    return task


def collect_tasks():
    if not os.path.isdir(SOURCE_ROOT):
        os.makedirs(SOURCE_ROOT, exist_ok=True)
        log("Created empty import folder: {}".format(SOURCE_ROOT))
        log("No local assets were found yet. See Docs/ASSET_IMPORT_PIPELINE_SR.md.")
        return [], []

    base_tasks = []
    animation_files = []
    skipped = []
    for current, dirs, files in os.walk(SOURCE_ROOT):
        dirs[:] = sorted(d for d in dirs if d not in {".git", "__pycache__", "COD2Extracted"})
        relative = os.path.relpath(current, SOURCE_ROOT)
        category = category_for(relative if relative != "." else "")
        destination = "{}/{}".format(DEST_ROOT, category)
        ensure_destination(destination)
        for filename in sorted(files):
            fullpath = os.path.join(current, filename)
            ext = os.path.splitext(filename)[1].lower()
            if ext in SUPPORTED_FBX and (category == "Animations" or filename.lower().startswith("an_")):
                animation_files.append((fullpath, destination, "Animations"))
            elif ext in SUPPORTED_FBX or ext in SUPPORTED_TEXTURES:
                base_tasks.append(make_task(fullpath, destination, category))
            elif ext in IGNORED_EXTENSIONS:
                skipped.append((fullpath, ext))
            elif filename.lower() in {"readme.txt", "readme.md", "readme_import_assets.txt"}:
                continue
            else:
                log("Unsupported file skipped: {}".format(fullpath))

    for path, ext in skipped:
        if ext in {".glb", ".gltf", ".obj", ".blend"}:
            log("Geometry file needs FBX conversion before UE import: {}".format(path))
        elif ext in {".xmodel", ".xanim"}:
            log("COD intermediate file needs the appropriate Blender/CoD import add-on first: {}".format(path))
        else:
            log("Archive deliberately skipped (never auto-import game archives): {}".format(path))
    return base_tasks, animation_files


def import_task_batch(tasks, label):
    if not tasks:
        return 0
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)
    count = 0
    for task in tasks:
        try:
            imported = task.get_editor_property("imported_object_paths")
        except Exception:
            imported = []
        count += len(imported)
        for asset_path in imported:
            log("Imported {}: {}".format(label, asset_path))
    return count


def main():
    ensure_destination(DEST_ROOT)
    base_tasks, animation_files = collect_tasks()
    if not base_tasks and not animation_files:
        log("No importable FBX/texture assets. The project will keep procedural fallback geometry.")
        return

    imported_count = import_task_batch(base_tasks, "base asset")
    # Import animations after the character mesh so the intended skeleton can be assigned.
    skeleton = unreal.load_asset("/Game/Cacilend/Imported/Characters/SK_COD2_Soldier_Skeleton")
    animation_tasks = []
    for filename, destination, category in animation_files:
        animation_tasks.append(make_task(filename, destination, category, skeleton=skeleton))
    if animation_files and not skeleton:
        log("No SK_COD2_Soldier_Skeleton asset found. Animation FBXs may create their own skeleton; reassign the correct skeleton in the import dialog if needed.")
    imported_count += import_task_batch(animation_tasks, "animation")
    unreal.EditorAssetLibrary.save_directory(DEST_ROOT, only_if_is_dirty=False, recursive=True)
    log("IMPORT FINISHED. Base tasks: {}; animation tasks: {}; reported objects: {}.".format(
        len(base_tasks), len(animation_tasks), imported_count
    ))
    log("Expected mesh paths: /Game/Cacilend/Imported/Buildings/SM_NationalAssembly, /Game/Cacilend/Imported/Buildings/SM_PionirskiPark, /Game/Cacilend/Imported/Buildings/SM_Presidency, /Game/Cacilend/Imported/Buildings/SM_MainPostOfficePalace, /Game/Cacilend/Imported/Characters/SK_COD2_Soldier, /Game/Cacilend/Imported/Weapons/SM_COD2_Weapon.")


if __name__ == "__main__":
    main()
