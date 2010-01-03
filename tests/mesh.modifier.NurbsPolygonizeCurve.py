#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsPolygonizeCurve")

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.segments = 10
setup.modifier.delete_original = True


testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsPolygonizeCurve", 2)

