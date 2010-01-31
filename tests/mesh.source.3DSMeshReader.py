#python

import testing

setup = testing.setup_mesh_reader_test("3DSMeshReader", "mesh.source.3DSMeshReader.test.3ds")

testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.source.get_property("output_mesh"), "mesh.source.3DSMeshReader.test", 1)

