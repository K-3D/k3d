#python

import testing

setup = testing.setup_mesh_reader_test("STLMeshReader", "mesh.source.STLMeshReader.stl")
testing.mesh_reference_comparison(setup.document, setup.reader.get_property("output_mesh"), "mesh.source.STLMeshReader", 1)

