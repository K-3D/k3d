#python

import k3d
import testing

setup = testing.setup_mesh_source_test("PolyDisk")
if k3d.polyhedron.is_solid(k3d.polyhedron.validate(setup.source.output_mesh.primitives()[0])):
  raise Exception("Expected non-solid polyhedron")

