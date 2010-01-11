#python

import testing

setup = testing.setup_mesh_source_test("Disk")


testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.Disk", 1)

