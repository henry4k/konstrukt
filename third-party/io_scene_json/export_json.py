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
        return (v[0], v[1], v[2])

class NormalAttribute(VertexAttribute):
    def __init__( self, data ):
        VertexAttribute.__init__(
            self,
            data_type='float',
            component_count=3)

        self.data = data

    def get_data( self, vertex_index ):
        v = self.data[vertex_index].normal
        return (v[0], v[1], v[2])

class ColorAttribute(VertexAttribute):
    def __init__( self, data ):
        VertexAttribute.__init__(
            self,
            data_type='float',
            component_count=3)

        self.data = data

    def get_data( self, vertex_index ):
        v = self.data[vertex_index].color
        return (v.r, v.g, v.b)

class TexCoordAttribute(VertexAttribute):
    def __init__( self, data ):
        VertexAttribute.__init__(
            self,
            data_type='float',
            component_count=2)

        self.data = data

    def get_data( self, vertex_index ):
        v = self.data[vertex_index].uv
        return (v[0], v[1])

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
    r = {}

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
        bmesh.ops.triangulate(bmesh, faces=bm.faces)
        bm.to_mesh(mesh)
        bm.free()


    # Add vertex format
    attributes = {}

    attributes['position'] = PositionAttribute(mesh.vertices)
    attributes['normal'] = NormalAttribute(mesh.vertices)

    for vertex_color in mesh.vertex_colors:
        attributes[vertex_color.name] = ColorAttribute(vertex_color.data)

    for uv_layer in mesh.uv_layers:
        attributes[uv_layer.name] = TexCoordAttribute(uv_layer.data)

    set_attribute_offsets(attributes)

    r['vertex_format'] = build_vertex_attribute_tree(attributes)
    r['primitive'] = 'triangles' # See TRIANGULATE


    # Add vertex data
    vertex_size = get_vertex_size(attributes)
    vertex_count = len(mesh.vertices)
    vertex_data = [0.0] * vertex_size * vertex_count

    for vertex in range(0, vertex_count):
        vertex_start = vertex*vertex_size

        for attribute in attributes.values():
            attribute_value = attribute.get_data(vertex)
            assert len(attribute_value) == attribute.component_count

            for component_index, component_value in enumerate(attribute_value):
                index = vertex_start + attribute.offset + component_index
                vertex_data[index] = component_value

    r['vertices'] = vertex_data

    face_count = len(mesh.polygons)
    faces = [ [0,0,0] ]*face_count
    for face in range(0, face_count):
        f = mesh.polygons[face].vertices
        faces[face] = (f[0], f[1], f[2])
    r['faces'] = faces

    return r


##### Object {{{1

def build_object(obj):
    tree = {}

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
    tree = {}
    for child in children:
        if is_blacklisted_object(child):
            continue
        tree[child.name] = build_object(child)
    return tree


##### Save operation {{{1

def save(operator,
         context,
         filepath=None,
         triangulate=False):

    global TRIANGULATE
    TRIANGULATE = triangulate

    scene = context.scene
    objects = scene.objects

    global SCENE
    SCENE = scene

    def filter_children(obj):
        return obj.parent == None
    tree = build_children(filter(filter_children, objects))

    file = open(filepath, 'w', encoding='utf8', newline='\n')
    file.write(json.dumps(tree, sort_keys=True, indent=4, separators=(',', ': ')))
    file.close()

    return {'FINISHED'}
