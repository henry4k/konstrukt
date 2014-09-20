# vim: set foldmethod=marker:

import os
import json


TRIANGULATE = False
SCENE = None
Y_IS_UP = False



##### Properties {{{1

def is_blacklisted_property(obj, name, value):
    for t in [float, int, bool, str]:
        if type(value) == t:
            return False
    return True

def build_properties(obj):
    properties = {}
    for property_index, property_name in enumerate(obj.keys()):
        property_value = obj.values()[property_index]
        if is_blacklisted_property(obj, property_name, property_value):
            continue
        properties[property_name] = property_value
    return properties


##### Mesh {{{1

def build_mesh(obj):
    r = dict()

    # Prepare mesh
    try:
        mesh = obj.to_mesh(
                scene=SCENE,
                apply_modifiers=True,
                settings='PREVIEW',
                calc_tessface=True)
    except RuntimeError:
        mesh = None

    if mesh is None:
        return None

    mesh.transform(obj.matrix_world)

    if TRIANGULATE:
        import bmesh
        bm = bmesh.new()
        bm.from_mesh(mesh)
        bmesh.ops.triangulate(bm, faces=bm.faces)
        bm.to_mesh(mesh)
        bm.free()

    mesh.calc_normals()

    if not mesh.tessfaces and mesh.polygons:
        mesh.calc_tessface()

    uv_layer = None
    if mesh.tessface_uv_textures.active:
        uv_layer = mesh.tessface_uv_textures.active.data

    color_layer = None
    if mesh.tessface_vertex_colors.active:
        color_layer = mesh.tessface_vertex_colors.active.data

    def rvec3d(v):
        return round(v[0], 6), round(v[1], 6), round(v[2], 6)

    def rvec2d(v):
        return round(v[0], 6), round(v[1], 6)

    vertices_out = []
    faces_out = [[] for f in range(len(mesh.tessfaces))]
    vdict = [{} for i in range(len(mesh.vertices))]
    vertex_count = 0

    color = uv = uv_key = normal = normal_key = None

    for face_index, face in enumerate(mesh.tessfaces):
        smooth = face.use_smooth
        if not smooth:
            normal = face.normal[:]
            normal_key = rvec3d(normal)

        if uv_layer:
            uvs = uv_layer[face_index]
            uvs = (uvs.uv1,
                   uvs.uv2,
                   uvs.uv3,
                   uvs.uv4)

        if color_layer:
            colors = color_layer[face_index]
            colors = (colors.color1[:],
                      colors.color2[:],
                      colors.color3[:],
                      colors.color4[:])

        face_out = faces_out[face_index]
        for face_vertex_index, vertex_index in enumerate(face.vertices):
            vertex = mesh.vertices[vertex_index]

            if smooth:
                normal = vertex.normal[:]
                normal_key = rvec3d(normal)

            if uv_layer:
                uv = (uvs[face_vertex_index][0],
                      uvs[face_vertex_index][1])
                uv_key = rvec2d(uv)

            if color_layer:
                color = colors[face_vertex_index]

            key = normal_key, uv_key, color

            vdict_local = vdict[vertex_index]
            face_out_vertex_index = vdict_local.get(key)

            if face_out_vertex_index is None:
                face_out_vertex_index = vdict_local[key] = vertex_count

                if Y_IS_UP:
                    vertex_out = dict(x=vertex.co[0],
                                      y=vertex.co[2],
                                      z=vertex.co[1],
                                      nx=normal[0],
                                      ny=normal[2],
                                      nz=normal[1])
                else:
                    vertex_out = dict(x=vertex.co[0],
                                      y=vertex.co[1],
                                      z=vertex.co[2],
                                      nx=normal[0],
                                      ny=normal[1],
                                      nz=normal[2])
                if uv_layer:
                    vertex_out.update(dict(tx=uv[0],
                                           ty=uv[1]))
                if color_layer:
                    vertex_out.update(dict(r=color[0],
                                           g=color[1],
                                           b=color[2]))
                vertices_out.append(vertex_out)
                vertex_count += 1

            face_out.append(face_out_vertex_index)

    r['vertices'] = vertices_out
    r['faces'] = faces_out

    return r


##### Object {{{1

def build_object(obj):
    tree = dict()

    if obj.type == 'MESH':
        mesh = build_mesh(obj)
        tree.update(mesh)

    # Add properties
    properties = build_properties(obj)
    tree.update(properties)

    # Add children
    children = build_children(obj.children)
    tree.update(children)

    return tree


##### Children {{{1

def is_blacklisted_object(obj):
    return False

def build_children(children):
    tree = dict()
    for child in children:
        if is_blacklisted_object(child):
            continue
        tree[child.name] = build_object(child)
    return tree


##### Save operation {{{1

def save(scene,
         filepath=None,
         triangulate=True,
         y_is_up=False,
         pretty_json=False):

    global TRIANGULATE
    TRIANGULATE = triangulate

    global SCENE
    SCENE = scene

    global Y_IS_UP
    Y_IS_UP = y_is_up

    def filter_children(obj):
        return obj.parent == None
    tree = build_children(filter(filter_children, scene.objects))

    file = open(filepath, 'w', encoding='utf8', newline='\n')
    if pretty_json:
        json.dump(tree, file, sort_keys=True, indent=4, separators=(',', ': '))
    else:
        json.dump(tree, file, separators=(',', ':'))
    file.close()

    return {'FINISHED'}
