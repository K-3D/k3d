#python

import k3d
import testing

setup = testing.setup_mesh_source_test("PolyCube")
if not k3d.polyhedron.is_solid(k3d.polyhedron.validate(setup.source.output_mesh.primitives()[0])):
  raise Exception("Expected solid polyhedron")

