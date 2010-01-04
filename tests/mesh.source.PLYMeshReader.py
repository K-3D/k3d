#python

import testing

setup = testing.setup_mesh_reader_test("PLYMeshReader", "mesh.source.PLYMeshReader.ply")

testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.mesh_reference_comparison(setup.document, setup.source.get_property("output_mesh"), "mesh.source.PLYMeshReader", 1)

