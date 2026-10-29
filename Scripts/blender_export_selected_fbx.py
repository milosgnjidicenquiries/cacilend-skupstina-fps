"""Export currently selected Blender objects as FBX for Unreal Engine.

Examples:
  blender --background imported_scene.blend --python Scripts/blender_export_selected_fbx.py -- output.fbx --static
  blender --background imported_scene.blend --python Scripts/blender_export_selected_fbx.py -- output.fbx

Select the desired model (and its armature for a character) in the saved .blend file.
Default mode bakes the current action/NLA animation where available; --static disables animation export.
"""
import os
import sys
import bpy


def arguments_after_separator():
    argv = sys.argv
    if "--" not in argv:
        raise RuntimeError("Pass an output FBX path after '--'.")
    return argv[argv.index("--") + 1:]


def main():
    args = arguments_after_separator()
    if not args:
        raise RuntimeError("Missing output FBX path.")
    output = os.path.abspath(args[0])
    static_only = "--static" in args[1:]
    selected = list(bpy.context.selected_objects)
    if not selected:
        raise RuntimeError("No Blender objects are selected. Select the mesh and armature, then save the .blend file.")

    os.makedirs(os.path.dirname(output) or ".", exist_ok=True)
    bpy.ops.export_scene.fbx(
        filepath=output,
        use_selection=True,
        object_types={"MESH", "ARMATURE"},
        apply_unit_scale=True,
        apply_scale_options="FBX_SCALE_ALL",
        bake_space_transform=False,
        axis_forward="-Z",
        axis_up="Y",
        use_mesh_modifiers=True,
        mesh_smooth_type="FACE",
        use_armature_deform_only=True,
        add_leaf_bones=False,
        path_mode="COPY",
        embed_textures=True,
        bake_anim=not static_only,
        bake_anim_use_all_bones=not static_only,
        bake_anim_use_nla_strips=not static_only,
        bake_anim_use_all_actions=False,
    )
    print("[Cacilend] Exported {} selected object(s) to {}".format(len(selected), output))


if __name__ == "__main__":
    main()
