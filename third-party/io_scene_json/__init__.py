bl_info = {
    "name": "JSON scene export",
    "author": "Henry Kielmann",
    "blender": (2, 57, 0),
    "version": (0,0,1),
    "location": "File > Export",
    "category": "Import-Export",
    "description": "Exports the scene as JSON document",
    "wiki_url": "http://github.com/henry4k/io_scene_json",
    "tracker_url": "http://github.com/henry4k/io_scene_json"}


import bpy
from bpy.props import StringProperty, FloatProperty, BoolProperty
from bpy_extras.io_utils import ExportHelper


# To support reload properly, try to access a package var, if it's there, reload everything
if "bpy" in locals():
    import imp
    if "export_json" in locals():
        imp.reload(export_json)

class ExportJSON(bpy.types.Operator, ExportHelper):
    bl_idname = "export_scene.json"
    bl_label = "Export JSON"
    bl_options = {'PRESET'}

    filename_ext = ".json"
    filter_glob = StringProperty(default="*.json", options={'HIDDEN'})

    def execute(self, context):
        from . import export_json

        if not self.filepath:
            raise Exception("filepath not set")

        keywords = self.as_keywords(ignore=("check_existing", "filter_glob"))

        return export_json.save(context.scene, **keywords)

def menu_func(self, context):
    self.layout.operator(ExportJSON.bl_idname, text="JSON (.json)")

def register():
    bpy.utils.register_module(__name__)
    bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
    bpy.utils.unregister_module(__name__)
    bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
    register()
