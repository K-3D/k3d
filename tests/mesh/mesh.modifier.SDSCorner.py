#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "SDSCorner")

selection = k3d.geometry.selection.create(0)
point_selection = k3d.geometry.point_selection.create(selection)
point_selection.index_begin().append(1)
point_selection.index_end().append(2)
point_selection.weight().append(1)

setup.modifier.mesh_selection = selection
setup.modifier.sharpness = 3.5


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.SDSCorner", 1)

