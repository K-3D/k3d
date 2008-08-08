#python

import testing

setup = testing.setup_mesh_reader_test("MD2MeshReader", "mesh.source.MD2MeshReader.dinosaur.md2")
testing.mesh_comparison_to_reference(setup.document, setup.reader.get_property("output_mesh"), "mesh.source.MD2MeshReader.dinosaur", 1)

