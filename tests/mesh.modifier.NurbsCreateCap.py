#python

import testing
import k3d

setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsCreateCap")

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsCreateCap", 1)

