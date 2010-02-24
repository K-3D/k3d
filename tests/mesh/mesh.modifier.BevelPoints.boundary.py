#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyGrid", "BevelPoints")

selection = k3d.selection.set()
point_selection = k3d.geometry.point_selection.create(selection, 1)

setup.modifier.mesh_selection = selection

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.BevelPoints.boundary", 1)

