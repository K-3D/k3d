#python

import testing

setup = testing.setup_mesh_source_test("NurbsCylinder")
testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.NurbsCylinder", 1, testing.platform_specific)

