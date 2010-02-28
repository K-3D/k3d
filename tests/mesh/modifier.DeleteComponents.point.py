#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "DeleteComponents")

selection = k3d.geometry.selection.create(0)
point_selection = k3d.geometry.point_selection.create(selection)
k3d.geometry.point_selection.append(point_selection, 0, 2, 1)
k3d.geometry.point_selection.append(point_selection, 14, 15, 1)
setup.modifier.mesh_selection = selection


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.DeleteComponents.point", 1)

