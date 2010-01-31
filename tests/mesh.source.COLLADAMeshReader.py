#python

import testing

setup = testing.setup_mesh_reader_test("COLLADAMeshReader", "mesh.source.COLLADAMeshReader.duck.dae")

testing.require_valid_mesh(setup.document, setup.source.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.source.get_property("output_mesh"), "mesh.source.COLLADAMeshReader.duck", 1)

