#python

import k3d
import sys
import testing

setup = testing.setup_mesh_writer_test(["FrozenMesh", "K3DMeshWriter"], "K3DMeshReader", "mesh.serialization.k3d")

mesh = setup.source.create_mesh()
primitive = mesh.primitives().create("test")

array_types = [ "k3d::bool_t", "k3d::color", "k3d::double_t", "k3d::imaterial*", "k3d::inode*", "k3d::int16_t", "k3d::int32_t", "k3d::int64_t", "k3d::int8_t", "k3d::matrix4", "k3d::normal3", "k3d::point2", "k3d::point3", "k3d::point4", "k3d::string_t", "k3d::texture3", "k3d::uint16_t", "k3d::uint32_t", "k3d::uint64_t", "k3d::uint8_t", "k3d::vector2", "k3d::vector3", "k3d::uint_t" ]
array_values = [ True, k3d.color(1, 2, 3), 1.0, None, None, 1, 2, 3, 4, k3d.identity3(), k3d.normal3(1, 2, 3), k3d.point2(1, 2), k3d.point3(1, 2, 3), k3d.point4(1, 2, 3, 4), "A String", k3d.texture3(1, 2, 3), 1, 2, 3, 4, k3d.vector2(1, 2), k3d.vector3(1, 2, 3), 7 ]

structure = primitive.structure().create("generic")
attributes = primitive.attributes().create("generic")

for i in range(len(array_types)):
	type = array_types[i]
	value = array_values[i]

	structure_array = structure.create(type, type)
	structure_array.append(value)
	structure_array.append(value)
	structure_array.append(value)

	attribute_array = attributes.create(type, type)
	attribute_array.append(value)
	attribute_array.append(value)
	attribute_array.append(value)

setup.source.set_mesh(mesh) # This is a bit of a hack, it forces the writer to send the current mesh to disk

result = k3d.difference.accumulator()
k3d.difference.test(setup.source.output_mesh, setup.reader.output_mesh, result)

if not (result.exact_min() == True and result.ulps_max() == 0):
	raise Exception("serialized mesh differs from reference")

