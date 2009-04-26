#python

import k3d
import sys

document = k3d.new_document()
frozen_mesh = document.new_node("FrozenMesh")
mesh = frozen_mesh.create_mesh()
polyhedron = k3d.polyhedron.create(mesh)

named_array_types = [ "k3d::bool_t", "k3d::color", "k3d::double_t", "k3d::inode*", "k3d::int16_t", "k3d::int32_t", "k3d::int64_t", "k3d::int8_t", "k3d::matrix4", "k3d::normal3", "k3d::point2", "k3d::point3", "k3d::point4", "k3d::string_t", "k3d::texture3", "k3d::uint16_t", "k3d::uint32_t", "k3d::uint64_t", "k3d::uint8_t", "k3d::vector2", "k3d::vector3", "k3d::uint_t" ]

for type in named_array_types:
	array = polyhedron.face_varying_data().create(type, type)

for type in named_array_types:
	array = polyhedron.face_varying_data()[type]

for type in named_array_types:
	const_array = polyhedron.face_varying_data()[type]

