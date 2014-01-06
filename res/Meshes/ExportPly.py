import bpy
import sys

argv = sys.argv
argv = argv[argv.index("--") + 1:] # get all args after "--"

out = argv[0]

bpy.ops.export_mesh.ply(
    filepath=out,
    use_mesh_modifiers=True,
    use_uv_coords=True,
    use_normals=True,
    use_colors=False,
    axis_forward='-Z',
    axis_up='Y',
    global_scale=1.0)
