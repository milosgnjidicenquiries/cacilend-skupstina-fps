"""Optional Google Photorealistic 3D Tiles setup for UE5 via Cesium for Unreal.

Needs:
- Cesium for Unreal plug-in compatible with this UE5 installation.
- Google Maps Platform project with Map Tiles API enabled and billing enabled.
- A restricted API key stored in Config/Private/GoogleMapsApiKey.txt or GOOGLE_MAPS_API_KEY.

This does not download/cache Google geometry into the repository.
"""
import os
import unreal

PROJECT_ROOT = str(unreal.Paths.project_dir()).rstrip("/\\")
KEY_FILE = os.path.join(PROJECT_ROOT, "Config", "Private", "GoogleMapsApiKey.txt")
ORIGIN_LATITUDE = 44.81076
ORIGIN_LONGITUDE = 20.46583
ORIGIN_HEIGHT = 160.0
ROOT_URL = "https://tile.googleapis.com/v1/3dtiles/root.json"


def log(message):
    unreal.log("[Cacilend Google 3D Tiles] " + str(message))


def try_set(obj, property_name, value):
    try:
        obj.set_editor_property(property_name, value)
        return True
    except Exception as exc:
        log("Could not set {}: {}".format(property_name, exc))
        return False


def get_key():
    env_key = os.environ.get("GOOGLE_MAPS_API_KEY", "").strip()
    if env_key:
        return env_key
    if os.path.isfile(KEY_FILE):
        with open(KEY_FILE, "r", encoding="utf-8") as handle:
            return handle.read().strip()
    return ""


def find_actor(class_name):
    try:
        actors = unreal.EditorLevelLibrary.get_all_level_actors()
    except Exception:
        return None
    for actor in actors:
        try:
            if actor.get_class().get_name() == class_name:
                return actor
        except Exception:
            continue
    return None


def set_georeference_origin(actor):
    placement_class = getattr(unreal, "EOriginPlacement", None)
    if placement_class:
        origin_value = getattr(placement_class, "CARTOGRAPHIC_ORIGIN", None)
        if origin_value is None:
            origin_value = getattr(placement_class, "CARTOGRAPHIC", None)
        if origin_value is not None:
            try_set(actor, "origin_placement", origin_value)

    method_values = (
        ("set_origin_latitude", ORIGIN_LATITUDE),
        ("set_origin_longitude", ORIGIN_LONGITUDE),
        ("set_origin_height", ORIGIN_HEIGHT),
    )
    for method_name, value in method_values:
        method = getattr(actor, method_name, None)
        if callable(method):
            try:
                method(value)
                continue
            except Exception as exc:
                log("Setter {} failed: {}".format(method_name, exc))
        property_name = method_name.replace("set_", "")
        try_set(actor, property_name, value)


def main():
    key = get_key()
    if not key:
        log("No API key: create Config/Private/GoogleMapsApiKey.txt. Skipping live 3D tiles; offline UE map remains usable.")
        return

    georef_class = unreal.load_class(None, "/Script/CesiumRuntime.CesiumGeoreference")
    tileset_class = unreal.load_class(None, "/Script/CesiumRuntime.Cesium3DTileset")
    if not georef_class or not tileset_class:
        log("Cesium for Unreal is not enabled/installed. Install the compatible plug-in, restart UE, then rerun Setup_Cacilend_Assets.bat.")
        return

    georef = find_actor("CesiumGeoreference")
    if not georef:
        georef = unreal.EditorLevelLibrary.spawn_actor_from_class(georef_class, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
        if georef:
            georef.set_actor_label("CesiumGeoreference_Belgrade")
    if not georef:
        log("Could not create CesiumGeoreference; use the manual setup in Docs/GOOGLE_EARTH_CESIUM_SETUP_SR.md.")
        return
    set_georeference_origin(georef)

    tileset = find_actor("Cesium3DTileset")
    if not tileset:
        tileset = unreal.EditorLevelLibrary.spawn_actor_from_class(tileset_class, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
        if tileset:
            tileset.set_actor_label("GooglePhotorealistic3DTiles_Belgrade")
    if not tileset:
        log("Could not create Cesium3DTileset; use the manual setup in Docs/GOOGLE_EARTH_CESIUM_SETUP_SR.md.")
        return

    enum_class = getattr(unreal, "ETilesetSource", None)
    source_value = getattr(enum_class, "FROM_URL", None) if enum_class else None
    if source_value is None and enum_class:
        source_value = getattr(enum_class, "FROM_URL", None)
    if source_value is None:
        log("Could not find Cesium From URL enum via Python. Set Source=From URL in the Details panel, then rerun this script.")
        return
    if not try_set(tileset, "tileset_source", source_value):
        log("Could not set Cesium tileset source to From URL. Configure that field manually in Details.")
        return

    url = ROOT_URL + "?key=" + key
    try_set(tileset, "url", url)
    try_set(tileset, "show_credits_on_screen", True)
    try_set(tileset, "create_physics_meshes", False)
    try_set(tileset, "create_nav_collision", False)
    try_set(tileset, "enable_occlusion_culling", True)
    tileset.set_actor_label("GooglePhotorealistic3DTiles_Belgrade")
    unreal.EditorLevelLibrary.save_current_level()
    log("Configured Google Photorealistic 3D Tiles around origin ({}, {}, {}). Verify visual alignment in the Editor and keep Google credits enabled.".format(
        ORIGIN_LATITUDE, ORIGIN_LONGITUDE, ORIGIN_HEIGHT
    ))


if __name__ == "__main__":
    main()
