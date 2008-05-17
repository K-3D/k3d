#python

import testing

setup = testing.setup_mesh_reader_test("OBJMeshReader", "mesh.source.OBJMeshReader.polyhedron.obj")
testing.mesh_comparison(setup.document, setup.reader.get_property("output_mesh"), "mesh.source.OBJMeshReader.polyhedron", 1)

