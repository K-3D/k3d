#python

import k3d
import testing

document = k3d.new_document()

source = k3d.plugin.create("FrozenMesh", document)
mesh = source.create_mesh()
polyhedron = k3d.polyhedron.create(mesh)
primitive = mesh.primitives()[0]

if k3d.polyhedron.validate(mesh, primitive):
	raise Exception("validate() should fail on polyhedron with missing points.")

mesh.create_points()

if k3d.polyhedron.validate(mesh, primitive):
	raise Exception("validate() should fail on polyhedron with missing point_selections.")

mesh.create_point_selection()

if not k3d.polyhedron.validate(mesh, primitive):
	raise Exception("validate() should succeed.")

polyhedron.clockwise_edges().append(0)
polyhedron.edge_selections().append(0)
polyhedron.vertex_points().append(0)
polyhedron.vertex_selections().append(0)

if k3d.polyhedron.validate(mesh, primitive):
	raise Exception("validate() should fail on polyhedron with out-of-bound point indices.")

mesh.points().append(k3d.point3(0, 0, 0))
mesh.point_selection().append(0)

if not k3d.polyhedron.validate(mesh, primitive):
	raise Exception("validate() should succeed.")

