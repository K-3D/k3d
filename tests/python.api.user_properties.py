#python

import k3d
import sys

document = k3d.new_document()
node = document.new_node("Null")

types = [ "k3d::bool_t", "k3d::color", "k3d::double_t", "k3d::filesystem::path", "k3d::gl::imesh_painter*", "k3d::imaterial*", "k3d::inode*", "k3d::int32_t", "k3d::legacy::mesh*", "k3d::matrix4", "k3d::mesh*", "k3d::normal3", "k3d::point3", "k3d::point4", "k3d::ri::imesh_painter*", "k3d::ri::itexture*", "k3d::string_t", "k3d::vector3" ]

skip_values = [ "k3d::legacy::mesh*" ]

for type in types:
	name = type
	label = type
	description = ""
	node.create_property(type, name, label, description)

for type in types:
	if type not in skip_values:
		name = type
		print type + " value:", node.get_property(name).internal_value()

