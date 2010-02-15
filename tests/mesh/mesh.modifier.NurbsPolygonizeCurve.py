#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsPolygonizeCurve")

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.segments = 10
setup.modifier.delete_original = True


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsPolygonizeCurve", 2)

