#python

import testing
import k3d

setup = testing.setup_mesh_modifier_test("NurbsCylinder","NurbsSetWeight")

setup.modifier.weight = 2.5
setup.modifier.mesh_selection = k3d.geometry.selection.create(1)

testing.require_valid_primitives(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsSetWeight2", 1)

