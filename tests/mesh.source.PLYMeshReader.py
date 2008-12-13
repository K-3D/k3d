#python

import testing

setup = testing.setup_mesh_reader_test("PLYMeshReader", "mesh.source.PLYMeshReader.ply")
testing.mesh_comparison_to_reference(setup.document, setup.reader.get_property("output_mesh"), "mesh.source.PLYMeshReader", 1)

