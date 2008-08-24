#python

import k3d

def test_equal(a, b, test):
	if not k3d.almost_equal(a, b, 0):
		raise Exception(test + " should test equal")

def test_unequal(a, b, test):
	if k3d.almost_equal(a, b, 0):
		raise Exception(test + " should test unequal")

document = k3d.new_document()

source_a = document.new_node("FrozenMesh")
source_b = document.new_node("FrozenMesh")

mesh_a = k3d.dynamic_cast(source_a, "imesh_storage").reset_mesh()
mesh_b = k3d.dynamic_cast(source_b, "imesh_storage").reset_mesh()

test_equal(mesh_a, mesh_b, "empty meshes")

points_a = mesh_a.create_points()
test_unequal(mesh_a, mesh_b, "mesh without point array")

points_b = mesh_b.create_points()
test_equal(mesh_a, mesh_b, "meshes with empty point arrays")

points_a.append(k3d.point3(1, 2, 3))
test_unequal(mesh_a, mesh_b, "mismatched point arrays")

points_b.append(k3d.point3(1, 2, 3))
test_equal(mesh_a, mesh_b, "matched point arrays")

primitive_a = mesh_a.primitives().create("foo")
test_unequal(mesh_a, mesh_b, "mesh without primitive")

primitive_b = mesh_b.primitives().create("foo")
test_equal(mesh_a, mesh_b, "matched primitives")

topology_a = primitive_a.topology().create("indices", "k3d::uint_t")
test_unequal(mesh_a, mesh_b, "topology with missing array")

topology_b = primitive_b.topology().create("indices", "k3d::uint_t")
test_equal(mesh_a, mesh_b, "topology with matched arrays")

topology_a.append(5)
test_unequal(mesh_a, mesh_b, "mismatched topology arrays")

topology_b.append(5)
test_equal(mesh_a, mesh_b, "matched topology_arrays")

attributes_a = primitive_a.attributes().create("uniform")
test_unequal(mesh_a, mesh_b, "primitive with missing attributes")

attributes_b = primitive_b.attributes().create("uniform")
test_equal(mesh_a, mesh_b, "matched primitive attributes")

attribute_a = attributes_a.create("Cs", "k3d::color")
test_unequal(mesh_a, mesh_b, "missing attribute arrays")

attribute_b = attributes_b.create("Cs", "k3d::color")
test_equal(mesh_a, mesh_b, "matched attribute arrays")

attribute_a.append(k3d.color(1, 2, 3))
test_unequal(mesh_a, mesh_b, "mismatched attributes")

attribute_b.append(k3d.color(1, 2, 3))
test_equal(mesh_a, mesh_b, "matched attributes")


