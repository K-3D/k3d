#python

import k3d

def test_equal(a, b, test):
	if not k3d.almost_equal(a, b, 0):
		print repr(a)
		print repr(b)
		raise Exception(test + " should test equal")

def test_unequal(a, b, test):
	if k3d.almost_equal(a, b, 0):
		print repr(a)
		print repr(b)
		raise Exception(test + " should test unequal")

document = k3d.new_document()

source_a = document.new_node("FrozenMesh")
source_b = document.new_node("FrozenMesh")

mesh_a = source_a.create_mesh()
mesh_b = source_b.create_mesh()

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

structure_a = primitive_a.structure().create("indices", "k3d::uint_t")
test_unequal(mesh_a, mesh_b, "structure with missing array")

structure_b = primitive_b.structure().create("indices", "k3d::uint_t")
test_equal(mesh_a, mesh_b, "structure with matched arrays")

structure_a.append(5)
test_unequal(mesh_a, mesh_b, "mismatched structure arrays")

structure_b.append(5)
test_equal(mesh_a, mesh_b, "matched structure_arrays")

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


