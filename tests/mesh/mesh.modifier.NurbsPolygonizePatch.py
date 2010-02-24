#python

import testing
import k3d

setup = testing.setup_mesh_modifier_test("NurbsSphere","NurbsPolygonizePatch")
setup.modifier.mesh_selection = k3d.geometry.selection.create(1)

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsPolygonizePatch", 1)
