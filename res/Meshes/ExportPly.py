import bpy
import sys

argv = sys.argv
argv = argv[argv.index("--") + 1:] # get all args after "--"

out = argv[0]

bpy.ops.export_mesh.ply(
    filepath=out,
    axis_forward='-Z',
    axis_up='Y',
    use_colors=False)
