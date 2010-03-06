#python

context.document.start_change_set()
try:
	node = context.document.new_node("Null")
	node.name = "User Properties Test"

	node.create_property("k3d::bool_t", "bool", "Boolean", "Boolean user property")
	node.create_property("k3d::color", "color", "Color", "Color user property")
	node.create_property("k3d::double_t", "double", "Double", "Double user property")
	node.create_property("k3d::int32_t", "integer", "Integer", "Integer user property")	
	node.create_property("k3d::matrix4", "matrix4", "Matrix4", "Matrix4 user property")	
	node.create_property("k3d::string_t", "string", "String", "String user property")	
	node.create_property("k3d::point3", "point3", "Point3", "Point user property")

	node.create_renderman_attribute("k3d::int32_t", "render", "ri_attribute_integer", "RenderMan Integer", "RenderMan Integer Attribute")
	node.create_renderman_attribute("k3d::double_t", "render", "ri_attribute_real", "RenderMan Real", "RenderMan Real Attribute")
	node.create_renderman_attribute("k3d::string_t", "render", "ri_attribute_string", "RenderMan String", "RenderMan String Attribute")
	node.create_renderman_attribute("k3d::point3", "render", "ri_attribute_point", "RenderMan Point", "RenderMan Point Attribute")
	node.create_renderman_attribute("k3d::vector3", "render2", "ri_attribute_vector", "RenderMan Vector", "RenderMan Vector Attribute")
	node.create_renderman_attribute("k3d::color", "render2", "ri_attribute_color", "RenderMan Color", "RenderMan Color Attribute")
	node.create_renderman_attribute("k3d::point4", "render2", "ri_attribute_hpoint", "RenderMan HPoint", "RenderMan HPoint Attribute")
	node.create_renderman_attribute("k3d::matrix4", "render2", "ri_attribute_matrix", "RenderMan Matrix", "RenderMan Matrix Attribute")

	node.string = "Howdy, World!"

	context.document.finish_change_set("User Property Sample")

except:
	context.document.cancel_change_set()
	raise

