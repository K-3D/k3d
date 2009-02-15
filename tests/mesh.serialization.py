#python

import k3d
import sys
import testing

document = k3d.new_document()

source = document.new_node("FrozenMesh")
mesh = source.create_mesh()
primitive = mesh.primitives().create("test")

array_types = [ "k3d::bool_t", "k3d::color", "k3d::double_t", "k3d::imaterial*", "k3d::inode*", "k3d::int16_t", "k3d::int32_t", "k3d::int64_t", "k3d::int8_t", "k3d::matrix4", "k3d::normal3", "k3d::point2", "k3d::point3", "k3d::point4", "k3d::string_t", "k3d::texture3", "k3d::uint16_t", "k3d::uint32_t", "k3d::uint64_t", "k3d::uint8_t", "k3d::vector2", "k3d::vector3", "k3d::uint_t" ]
array_values = [ True, k3d.color(1, 2, 3), 1.0, None, None, 1, 2, 3, 4, k3d.identity3(), k3d.normal3(1, 2, 3), k3d.point2(1, 2), k3d.point3(1, 2, 3), k3d.point4(1, 2, 3, 4), "A String", k3d.texture3(1, 2, 3), 1, 2, 3, 4, k3d.vector2(1, 2), k3d.vector3(1, 2, 3), 7 ]

attributes = primitive.attributes().create("uniform")
for i in range(len(array_types)):
	type = array_types[i]
	value = array_values[i]

	named_array = primitive.structure().create(type, type)
	named_array.append(value)
	named_array.append(value)

	attribute_array = attributes.create(type, type)
	attribute_array.append(value)
	attribute_array.append(value)
	attribute_array.append(value)

print repr(mesh)
sys.stdout.flush()

mesh_writer = document.new_node("K3DMeshWriter")
mesh_writer.file = k3d.generic_path(testing.binary_path() + "/mesh.serialization.output.k3d")
document.set_dependency(mesh_writer.get_property("input_mesh"), source.get_property("output_mesh"))

mesh_reader = document.new_node("K3DMeshReader")
mesh_reader.file = mesh_writer.file

print repr(mesh_reader.output_mesh)
sys.stdout.flush()

difference = testing.get_mesh_difference(document, source.get_property("output_mesh"), mesh_reader.get_property("output_mesh"), 0)
if not difference.equal:
	testing.output_mesh_difference(source.output_mesh, mesh_reader.output_mesh, 0)
	raise Exception("serialized mesh differs from reference")

