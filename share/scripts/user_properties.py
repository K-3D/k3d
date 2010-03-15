#python

import k3d

context.document.start_change_set()
try:
	node = k3d.plugin.create("Null", context.document)
	node.name = "User Properties Test"

	k3d.property.create(node, "k3d::bool_t", "bool", "Boolean", "Boolean user property")
	k3d.property.create(node, "k3d::color", "color", "Color", "Color user property")
	k3d.property.create(node, "k3d::double_t", "double", "Double", "Double user property")
	k3d.property.create(node, "k3d::int32_t", "integer", "Integer", "Integer user property")	
	k3d.property.create(node, "k3d::matrix4", "matrix4", "Matrix4", "Matrix4 user property")	
	k3d.property.create(node, "k3d::string_t", "string", "String", "String user property")	
	k3d.property.create(node, "k3d::point3", "point3", "Point3", "Point user property")

	k3d.property.ri.create_attribute(node, "k3d::int32_t", "render", "ri_attribute_integer", "RenderMan Integer", "RenderMan Integer Attribute")
	k3d.property.ri.create_attribute(node, "k3d::double_t", "render", "ri_attribute_real", "RenderMan Real", "RenderMan Real Attribute")
	k3d.property.ri.create_attribute(node, "k3d::string_t", "render", "ri_attribute_string", "RenderMan String", "RenderMan String Attribute")
	k3d.property.ri.create_attribute(node, "k3d::point3", "render", "ri_attribute_point", "RenderMan Point", "RenderMan Point Attribute")
	k3d.property.ri.create_attribute(node, "k3d::vector3", "render2", "ri_attribute_vector", "RenderMan Vector", "RenderMan Vector Attribute")
	k3d.property.ri.create_attribute(node, "k3d::color", "render2", "ri_attribute_color", "RenderMan Color", "RenderMan Color Attribute")
	k3d.property.ri.create_attribute(node, "k3d::point4", "render2", "ri_attribute_hpoint", "RenderMan HPoint", "RenderMan HPoint Attribute")
	k3d.property.ri.create_attribute(node, "k3d::matrix4", "render2", "ri_attribute_matrix", "RenderMan Matrix", "RenderMan Matrix Attribute")

	node.string = "Howdy, World!"

	context.document.finish_change_set("User Property Sample")

except:
	context.document.cancel_change_set()
	raise

