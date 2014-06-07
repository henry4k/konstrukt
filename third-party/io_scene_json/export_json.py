# vim: set foldmethod=marker:

import os
import json


TRIANGULATE = False
SCENE = None



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

class VertexAttribute():
    def __init__( self, data_type, component_count ):
        self.data_type = data_type
        self.component_count = component_count
        self.offset = 0

    def get_data( self, vertex_index ):
        pass

class PositionAttribute(VertexAttribute):
    def __init__( self, data ):
        VertexAttribute.__init__(
            self,
            data_type='float',
            component_count=3)

        self.data = data

    def get_data( self, vertex_index ):
        v = self.data[vertex_index].co
        return dict(x=v[0], y=v[1], z=v[2])

class NormalAttribute(VertexAttribute):
    def __init__( self, data ):
        VertexAttribute.__init__(
            self,
            data_type='float',
            component_count=3)

        self.data = data

    def get_data( self, vertex_index ):
        v = self.data[vertex_index].normal
        return dict(nx=v[0], ny=v[1], nz=v[2])

class ColorAttribute(VertexAttribute):
    def __init__( self, data ):
        VertexAttribute.__init__(
            self,
            data_type='float',
            component_count=3)

        self.data = data

    def get_data( self, vertex_index ):
        v = self.data[vertex_index].color
        return dict(r=v.r, g=v.g, b=v.b)

class TexCoordAttribute(VertexAttribute):
    def __init__( self, data ):
        VertexAttribute.__init__(
            self,
            data_type='float',
            component_count=2)

        self.data = data

    def get_data( self, vertex_index ):
        v = self.data[vertex_index].uv
        return dict(tx=v[0], ty=v[1])

def set_attribute_offsets( attributes ):
    offset = 0
    for attribute in attributes.values():
        attribute.offset = offset
        offset = offset + attribute.component_count

def get_vertex_size( attributes ):
    size = 0
    for attribute in attributes.values():
        size = size + attribute.component_count
    return size

def build_vertex_attribute_tree( attributes ):
    tree = {}
    for attribute_name, attribute in attributes.items():
        tree[attribute_name] = {
            'data_type': attribute.data_type,
            'component_count': attribute.component_count,
            'offset': attribute.offset }
    return tree

def build_mesh(obj):
    r = dict()

    # Prepare mesh
    try:
        mesh = obj.to_mesh(
                scene=SCENE,
                apply_modifiers=True,
                settings='PREVIEW',
                calc_tessface=False)
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
    else:
        raise RuntimeError('Only triangulated meshes are supported!')


    attributes = dict()

    attributes['position'] = PositionAttribute(mesh.vertices)
    attributes['normal'] = NormalAttribute(mesh.vertices)

    for vertex_color in mesh.vertex_colors:
        attributes[vertex_color.name] = ColorAttribute(vertex_color.data)

    for uv_layer in mesh.uv_layers:
        attributes[uv_layer.name] = TexCoordAttribute(uv_layer.data)

    set_attribute_offsets(attributes)

    r['primitive'] = 'triangles' # See TRIANGULATE


    # Add vertices
    vertices = list()
    for vertex_index in range(0, len(mesh.vertices)):
        vertex = mesh.vertices[vertex_index]
        vertex_data = {}
        for attribute in attributes.values():
            attribute_data = attribute.get_data(vertex_index)
            vertex_data.update(attribute_data)
        vertices.append(vertex_data)
    r['vertices'] = vertices


    # Add indices
    indices = list()
    for face in mesh.polygons:
        for index in face.vertices:
            indices.append(index)
    r['indices'] = indices

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
         triangulate=True):

    global TRIANGULATE
    TRIANGULATE = triangulate

    objects = scene.objects

    global SCENE
    SCENE = scene

    def filter_children(obj):
        return obj.parent == None
    tree = build_children(filter(filter_children, objects))

    file = open(filepath, 'w', encoding='utf8', newline='\n')
    #json.dump(tree, file, sort_keys=True, indent=4, separators=(',', ': '))
    json.dump(tree, file, separators=(',',':'))
    file.close()

    return {'FINISHED'}
