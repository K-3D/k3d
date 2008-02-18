#python

import k3d
import sys

document = k3d.new_document()
node = document.new_node("Null")

types = [ "k3d::color", "k3d::point4", "k3d::int32_t", "k3d::matrix4", "k3d::normal3", "k3d::point3", "k3d::double_t", "k3d::string_t", "k3d::vector3" ]

for type in types:
	option_name = "test"
	name = type
	label = type
	description = ""
	node.create_renderman_option(type, option_name, name, label, description)

for type in types:
	name = type
	print type + " value:", node.get_property(name).internal_value()

