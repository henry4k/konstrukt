import os
import sys
import bpy

current_dir = os.path.dirname(__file__)
module_path = os.path.join(current_dir,'..','third-party','io_scene_json')
module_path = os.path.abspath(module_path)
sys.path.append(module_path)

import export_json


argv = sys.argv
argv = argv[argv.index("--") + 1:] # get all args after "--"

out = argv[0]

bpy.ops.object.select_all(action='DESELECT')

export_json.save(
    bpy.context.scene,
    filepath=out,
    triangulate=True,
    y_is_up=True)
