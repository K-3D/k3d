#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "SDSCrease")

selection = k3d.geometry.selection.create(0)
edge_selection = k3d.geometry.primitive_selection.create(selection)
edge_selection.primitive_begin().append(0)
edge_selection.primitive_end().append(1)
edge_selection.primitive_selection_type().append(k3d.selection.type.EDGE)
edge_selection.primitive_first_range().append(0)
edge_selection.primitive_range_count().append(1)
edge_selection.index_begin().append(2)
edge_selection.index_end().append(3)
edge_selection.weight().append(1)

setup.modifier.mesh_selection = selection
setup.modifier.sharpness = 2.5


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SDSCrease", 1)

