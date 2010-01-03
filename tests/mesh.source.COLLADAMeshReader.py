#python

import testing

setup = testing.setup_mesh_reader_test("COLLADAMeshReader", "mesh.source.COLLADAMeshReader.duck.dae")
testing.mesh_reference_comparison(setup.document, setup.reader.get_property("output_mesh"), "mesh.source.COLLADAMeshReader.duck", 1)

