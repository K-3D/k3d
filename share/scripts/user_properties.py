#python

doc = Document
doc.start_change_set()
try:
	cone = doc.new_node("Cone")
	cone.name = "IceCreamCone"

	cone.create_property("k3d::bool_t", "bool", "Boolean", "Boolean user property")
	cone.create_property("k3d::color", "color", "Color", "Color user property")
	cone.create_property("k3d::double_t", "double", "Double", "Double user property")
	cone.create_property("k3d::int32_t", "integer", "Integer", "Integer user property")	
	cone.create_property("k3d::matrix4", "matrix4", "Matrix4", "Matrix4 user property")	
	cone.create_property("k3d::string_t", "string", "String", "String user property")	
	cone.create_property("k3d::point3", "point3", "Point3", "Point user property")

	cone.add_ri_attribute("k3d::ri::integer", "render", "ri_attribute_integer", "RenderMan Integer", "RenderMan Integer Attribute")
	cone.add_ri_attribute("k3d::ri::real", "render", "ri_attribute_real", "RenderMan Real", "RenderMan Real Attribute")
	cone.add_ri_attribute("k3d::ri::string", "render", "ri_attribute_string", "RenderMan String", "RenderMan String Attribute")
	cone.add_ri_attribute("k3d::ri::point", "render", "ri_attribute_point", "RenderMan Point", "RenderMan Point Attribute")
	cone.add_ri_attribute("k3d::ri::vector", "render2", "ri_attribute_vector", "RenderMan Vector", "RenderMan Vector Attribute")
	cone.add_ri_attribute("k3d::ri::color", "render2", "ri_attribute_color", "RenderMan Color", "RenderMan Color Attribute")
	cone.add_ri_attribute("k3d::ri::hpoint", "render2", "ri_attribute_hpoint", "RenderMan HPoint", "RenderMan HPoint Attribute")
	cone.add_ri_attribute("k3d::ri::matrix", "render2", "ri_attribute_matrix", "RenderMan Matrix", "RenderMan Matrix Attribute")

	cone.string = "Howdy, World!"

	doc.finish_change_set("User Property Sample")

	doc.redraw_all()

except:
	doc.cancel_change_set()
	raise

