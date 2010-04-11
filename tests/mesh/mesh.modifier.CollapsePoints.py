#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "CollapsePoints")

selection = k3d.selection.set()
point_selection = k3d.geometry.point_selection.create(selection)
k3d.geometry.point_selection.append(point_selection, 14, 16, 1)
k3d.geometry.point_selection.append(point_selection, 20, 22, 1)
setup.modifier.mesh_selection = selection

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.CollapsePoints", 2)

