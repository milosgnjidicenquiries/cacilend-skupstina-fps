"""One-time editor bootstrap for the Cacilend Windows UE 5.8 prototype.
Run from Unreal Editor: Tools > Execute Python Script, selecting this file.
"""
import os
import unreal

ROOT = str(unreal.Paths.project_dir()).rstrip('/\\')
if not ROOT:
    ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
TEXTURE_SOURCE = os.path.join(ROOT, 'SourceArt', 'Textures')
TEXTURE_DEST = '/Game/Cacilend/Textures'
MATERIAL_DEST = '/Game/Cacilend/Materials'
MAP_PATH = '/Game/Maps/L_Cacilend'

STATUS_FILE = os.path.join(ROOT, 'Build', 'UE5-setup-status.txt')


def write_setup_status(status, detail=''):
    """Write a small status marker so the Windows launcher can verify setup completion."""
    try:
        os.makedirs(os.path.dirname(STATUS_FILE), exist_ok=True)
        with open(STATUS_FILE, 'w', encoding='utf-8') as handle:
            handle.write(str(status).strip() + os.linesep)
            if detail:
                handle.write(str(detail).replace(chr(13), ' ').replace(chr(10), ' ') + os.linesep)
    except Exception as status_error:
        log('Could not write setup status marker: ' + str(status_error))

MATERIALS = {
    'Asphalt': (4.0, 4.0, 0.92, 0.0),
    'Concrete': (3.0, 3.0, 0.82, 0.0),
    'Limestone': (2.5, 2.5, 0.69, 0.0),
    'Canvas': (2.0, 2.0, 0.91, 0.0),
    'Grass': (3.5, 3.5, 0.96, 0.0),
    'Wood': (2.0, 2.0, 0.58, 0.0),
    'Metal': (2.0, 2.0, 0.46, 0.48),
    'Glass': (1.4, 1.4, 0.18, 0.12),
    'InteriorWood': (2.2, 2.2, 0.43, 0.0),
    'Carpet': (3.0, 3.0, 0.92, 0.0),
    'DarkFabric': (2.3, 2.3, 0.90, 0.0),
}


def log(msg):
    unreal.log('[Cacilend Setup] ' + str(msg))


def ensure_dirs():
    tools = unreal.EditorAssetLibrary
    tools.make_directory('/Game/Cacilend')
    tools.make_directory(TEXTURE_DEST)
    tools.make_directory(MATERIAL_DEST)
    tools.make_directory('/Game/Cacilend/Imported')
    for category in ('Buildings', 'Characters', 'Weapons', 'Animations', 'Maps', 'COD2Maps', 'Textures', 'Materials', 'Vehicles', 'Props'):
        tools.make_directory(f'/Game/Cacilend/Imported/{category}')
    tools.make_directory('/Game/Maps')


def import_textures():
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    tasks = []
    if not os.path.isdir(TEXTURE_SOURCE):
        log('SourceArt/Textures was not found; run Scripts/generate_textures.py first.')
        return
    for filename in sorted(os.listdir(TEXTURE_SOURCE)):
        if not filename.lower().endswith('.png'):
            continue
        task = unreal.AssetImportTask()
        task.set_editor_property('filename', os.path.join(TEXTURE_SOURCE, filename))
        task.set_editor_property('destination_path', TEXTURE_DEST)
        task.set_editor_property('automated', True)
        task.set_editor_property('replace_existing', True)
        task.set_editor_property('save', True)
        tasks.append(task)
    if tasks:
        asset_tools.import_asset_tasks(tasks)
    for material_name in MATERIALS:
        for suffix in ('BaseColor', 'Normal', 'Roughness'):
            obj = unreal.load_asset(f'{TEXTURE_DEST}/T_{material_name}_{suffix}')
            if not obj:
                log(f'WARNING: failed to import T_{material_name}_{suffix}')
                continue
            try:
                obj.set_editor_property('srgb', suffix == 'BaseColor')
                if suffix == 'Normal':
                    obj.set_editor_property('compression_settings', unreal.TextureCompressionSettings.TC_NORMALMAP)
                unreal.EditorAssetLibrary.save_loaded_asset(obj)
            except Exception as ex:
                log(f'Texture setting note for {suffix}: {ex}')
    log('Imported source PBR texture maps.')


def set_prop_if_exists(obj, name, value):
    try:
        obj.set_editor_property(name, value)
        return True
    except Exception:
        return False


def expr(material, cls, x, y):
    return unreal.MaterialEditingLibrary.create_material_expression(material, cls, x, y)


def connect(a, out_pin, b, in_pin):
    unreal.MaterialEditingLibrary.connect_material_expressions(a, out_pin, b, in_pin)


def create_material(name, u_tiling, v_tiling, default_roughness, metallic):
    asset_path = f'{MATERIAL_DEST}/M_{name}'
    if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
        # Preserve the existing asset object/path so placed map components keep
        # their material references when setup is rerun.
        mat = unreal.load_asset(asset_path)
        if not mat:
            log(f'ERROR: existing M_{name} could not be loaded')
            return
        try:
            unreal.MaterialEditingLibrary.delete_all_material_expressions(mat)
        except Exception as ex:
            log(f'ERROR: could not reset M_{name} graph safely: {ex}')
            return
    else:
        tools = unreal.AssetToolsHelpers.get_asset_tools()
        mat = tools.create_asset(f'M_{name}', MATERIAL_DEST, unreal.Material, unreal.MaterialFactoryNew())
        if not mat:
            log(f'ERROR: could not create M_{name}')
            return

    base_tex = unreal.load_asset(f'{TEXTURE_DEST}/T_{name}_BaseColor')
    norm_tex = unreal.load_asset(f'{TEXTURE_DEST}/T_{name}_Normal')
    rough_tex = unreal.load_asset(f'{TEXTURE_DEST}/T_{name}_Roughness')
    uv = expr(mat, unreal.MaterialExpressionTextureCoordinate, -1050, 20)
    uv_scale = expr(mat, unreal.MaterialExpressionScalarParameter, -850, 120)
    set_prop_if_exists(uv_scale, 'parameter_name', 'UVScale')
    set_prop_if_exists(uv_scale, 'default_value', 1.0)
    uv_multiply = expr(mat, unreal.MaterialExpressionMultiply, -650, 20)
    connect(uv, '', uv_multiply, 'A')
    connect(uv_scale, '', uv_multiply, 'B')

    tint = expr(mat, unreal.MaterialExpressionVectorParameter, -650, 100)
    set_prop_if_exists(tint, 'parameter_name', 'Tint')
    set_prop_if_exists(tint, 'default_value', unreal.LinearColor(1.0, 1.0, 1.0, 1.0))
    if base_tex:
        base = expr(mat, unreal.MaterialExpressionTextureSample, -420, -180)
        base.set_editor_property('texture', base_tex)
        connect(uv_multiply, '', base, 'Coordinates')
        mul = expr(mat, unreal.MaterialExpressionMultiply, -350, -100)
        connect(base, 'RGB', mul, 'A')
        connect(tint, '', mul, 'B')
        unreal.MaterialEditingLibrary.connect_material_property(mul, '', unreal.MaterialProperty.MP_BASE_COLOR)
    else:
        # Keep visible neutral materials even when optional Python texture tools or
        # generated PBR files are unavailable. Runtime mesh components tint this
        # parameter to produce the map's limestone, asphalt, concrete, etc.
        unreal.MaterialEditingLibrary.connect_material_property(
            tint, '', unreal.MaterialProperty.MP_BASE_COLOR
        )

    if norm_tex:
        normal = expr(mat, unreal.MaterialExpressionTextureSample, -420, 350)
        normal.set_editor_property('texture', norm_tex)
        set_prop_if_exists(normal, 'sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_NORMAL)
        connect(uv_multiply, '', normal, 'Coordinates')
        unreal.MaterialEditingLibrary.connect_material_property(normal, 'RGB', unreal.MaterialProperty.MP_NORMAL)

    if rough_tex:
        rough = expr(mat, unreal.MaterialExpressionTextureSample, -420, 600)
        rough.set_editor_property('texture', rough_tex)
        set_prop_if_exists(rough, 'sampler_type', unreal.MaterialSamplerType.SAMPLERTYPE_LINEAR_COLOR)
        connect(uv_multiply, '', rough, 'Coordinates')
        unreal.MaterialEditingLibrary.connect_material_property(rough, 'R', unreal.MaterialProperty.MP_ROUGHNESS)
    else:
        rough_const = expr(mat, unreal.MaterialExpressionConstant, -300, 600)
        set_prop_if_exists(rough_const, 'r', float(default_roughness))
        unreal.MaterialEditingLibrary.connect_material_property(rough_const, '', unreal.MaterialProperty.MP_ROUGHNESS)

    metal_expr = expr(mat, unreal.MaterialExpressionConstant, -300, 300)
    metal_expr.set_editor_property('r', float(metallic))
    unreal.MaterialEditingLibrary.connect_material_property(metal_expr, '', unreal.MaterialProperty.MP_METALLIC)
    set_prop_if_exists(mat, 'two_sided', name in ('Canvas', 'DarkFabric', 'Grass'))
    try:
        unreal.MaterialEditingLibrary.recompile_material(mat)
    except Exception:
        pass
    unreal.EditorAssetLibrary.save_loaded_asset(mat)
    log(f'Created material M_{name}')


def import_external_assets():
    """Import optional local FBX/texture assets before the generated level is played."""
    script_path = os.path.join(ROOT, 'Content', 'Python', 'Import_Cacilend_Assets.py')
    if not os.path.isfile(script_path):
        log('Import_Cacilend_Assets.py not found; using procedural fallback meshes.')
        return
    try:
        namespace = {'__name__': '__main__', '__file__': script_path}
        with open(script_path, 'r', encoding='utf-8') as handle:
            source = handle.read()
        exec(compile(source, script_path, 'exec'), namespace, namespace)
        log('External asset import step finished.')
    except Exception as ex:
        log('WARNING: external import failed; procedural fallback will remain available: ' + str(ex))


def build_materials():
    for name, (u, v, rough, metal) in MATERIALS.items():
        create_material(name, u, v, rough, metal)
    log('PBR material library created.')


def spawn_if_class(class_obj, location, rotation=unreal.Rotator(0, 0, 0)):
    return unreal.EditorLevelLibrary.spawn_actor_from_class(class_obj, location, rotation)


def build_level():
    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        unreal.EditorLevelLibrary.load_level(MAP_PATH)
        for actor in unreal.EditorLevelLibrary.get_all_level_actors():
            if actor.get_class().get_name() == 'CacilendMapBuilder':
                log('Map builder is already in the level; keeping existing level actors.')
                return
    else:
        unreal.EditorLevelLibrary.new_level(MAP_PATH)

    builder_class = unreal.load_class(None, '/Script/CacilendUE.CacilendMapBuilder')
    if not builder_class:
        raise RuntimeError('Cacilend C++ module was not found. Compile the project in Unreal/Visual Studio, then rerun this script.')
    spawn_if_class(builder_class, unreal.Vector(0, 0, 0))
    player_start = spawn_if_class(unreal.PlayerStart, unreal.Vector(-8500, -7200, 105), unreal.Rotator(0, 90, 0))
    if player_start:
        player_start.set_actor_label('FPS_Start_Approach')

    sun = spawn_if_class(unreal.DirectionalLight, unreal.Vector(0, 0, 8000), unreal.Rotator(-38, -35, 0))
    if sun:
        sun.set_actor_label('Sun_Directional')
        light_component = sun.get_component_by_class(unreal.DirectionalLightComponent)
        if light_component:
            set_prop_if_exists(light_component, 'intensity', 100000.0)
            set_prop_if_exists(light_component, 'use_temperature', True)
            set_prop_if_exists(light_component, 'temperature', 5600.0)
            set_prop_if_exists(light_component, 'cast_shadows', True)

    sky = spawn_if_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0))
    if sky:
        sky.set_actor_label('Sky_Atmosphere')
    sky_light = spawn_if_class(unreal.SkyLight, unreal.Vector(0, 0, 500))
    if sky_light:
        sky_light.set_actor_label('Sky_Light')
        sky_component = sky_light.get_component_by_class(unreal.SkyLightComponent)
        if sky_component:
            set_prop_if_exists(sky_component, 'real_time_capture', True)
            set_prop_if_exists(sky_component, 'intensity', 0.8)
    fog = spawn_if_class(unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0))
    if fog:
        fog.set_actor_label('Atmospheric_Haze')
        fog_component = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
        if fog_component:
            set_prop_if_exists(fog_component, 'fog_density', 0.012)
            set_prop_if_exists(fog_component, 'fog_height_falloff', 0.22)
    post = spawn_if_class(unreal.PostProcessVolume, unreal.Vector(0, 0, 0))
    if post:
        post.set_actor_label('Global_PostProcess')
        set_prop_if_exists(post, 'unbound', True)
        set_prop_if_exists(post, 'priority', 1.0)
        settings = post.get_editor_property('settings')
        set_prop_if_exists(settings, 'motion_blur_amount', 0.15)
        set_prop_if_exists(settings, 'bloom_intensity', 0.32)
        set_prop_if_exists(settings, 'vignette_intensity', 0.22)
        post.set_editor_property('settings', settings)

    unreal.EditorLevelLibrary.save_current_level()
    unreal.EditorAssetLibrary.save_directory('/Game/Maps', only_if_is_dirty=False, recursive=True)
    log('Level created and saved: ' + MAP_PATH)


def validate_level_setup():
    """Fail the setup if its saved map asset or essential gameplay actors are missing."""
    if not unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        raise RuntimeError('Generated map asset is missing: ' + MAP_PATH)

    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    class_names = {actor.get_class().get_name() for actor in actors}
    required = {'CacilendMapBuilder', 'PlayerStart'}
    missing = sorted(required - class_names)
    if missing:
        raise RuntimeError('Generated level is missing required actors: ' + ', '.join(missing))

    log('Level validation passed: map asset exists and required actors are present.')


def setup_google_tiles_optional():
    """Automatically configure Cesium only when its plug-in and a local API key exist."""
    script_path = os.path.join(ROOT, 'Content', 'Python', 'Setup_Google3DTiles_Cesium.py')
    if not os.path.isfile(script_path):
        log('Optional Cesium setup script not found; skipping live Google 3D Tiles.')
        return
    try:
        namespace = {'__name__': '__main__', '__file__': script_path}
        with open(script_path, 'r', encoding='utf-8') as handle:
            source = handle.read()
        exec(compile(source, script_path, 'exec'), namespace, namespace)
    except Exception as ex:
        log('Optional Google 3D Tiles setup skipped: ' + str(ex))
        log('The offline Belgrade map remains usable without Cesium or a Google API key.')


def main():
    ensure_dirs()
    import_textures()
    import_external_assets()
    build_materials()
    build_level()
    setup_google_tiles_optional()
    validate_level_setup()
    log('SETUP COMPLETE. Press Play in Unreal Editor to test the Windows FPS prototype.')


try:
    main()
    write_setup_status('SUCCESS', 'Saved level: ' + MAP_PATH)
except Exception as setup_error:
    write_setup_status('FAILED', repr(setup_error))
    log('FATAL setup error: ' + repr(setup_error))
    raise
