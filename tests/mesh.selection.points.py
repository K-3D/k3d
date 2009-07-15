#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "ScalePoints")

setup.source.rows = 1
setup.source.columns = 1

mesh_selection = k3d.geometry.uniform_selection(0.0)
point_selection = k3d.geometry.point_selection.create(mesh_selection)
point_selection.index_begin().append(0)
point_selection.index_end().append(2)
point_selection.weight().append(1)

setup.modifier.mesh_selection = mesh_selection
setup.modifier.x = 2

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.selection.points", 1)

