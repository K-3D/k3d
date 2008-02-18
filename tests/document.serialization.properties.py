#python

import k3d
import testing

def create_property(node, type, value):
	property = node.create_property(type, type, "", "")
	property.set_value(value)

document = k3d.new_document()
test_container = document.new_node("Null")
test_container.name = "test_container"

test_material = document.new_node("MultiMaterial")
test_material.name = "test_material"

test_gl_mesh_painter = document.new_node("OpenGLPointPainter")
test_gl_mesh_painter.name = "test_gl_mesh_painter"

test_ri_mesh_painter = document.new_node("RenderManLinearCurvePainter")
test_ri_mesh_painter.name = "test_ri_mesh_painter"

test_ri_texture = document.new_node("RenderManTextureMap")
test_ri_texture.name = "test_ri_texture"

test_node = document.new_node("Axes")
test_node.name = "test_node"

create_property(test_container, "k3d::bool_t", True)
create_property(test_container, "k3d::color", k3d.color(1, 2, 3))
create_property(test_container, "k3d::double_t", 3.1415)
create_property(test_container, "k3d::filesystem::path", k3d.generic_path("/foo/bar/baz"))
create_property(test_container, "k3d::gl::imesh_painter*", test_gl_mesh_painter)
create_property(test_container, "k3d::imaterial*", test_material)
create_property(test_container, "k3d::inode*", test_node)
create_property(test_container, "k3d::int32_t", 19700827)
create_property(test_container, "k3d::matrix4", k3d.scale3(1, 2, 3))
create_property(test_container, "k3d::normal3", k3d.normal3(1, 2, 3))
create_property(test_container, "k3d::point3", k3d.point3(1, 2, 3))
create_property(test_container, "k3d::point4", k3d.point4(1, 2, 3, 4))
create_property(test_container, "k3d::ri::imesh_painter*", test_ri_mesh_painter)
create_property(test_container, "k3d::ri::itexture*", test_ri_texture)
create_property(test_container, "k3d::string_t", "K-3D Rocks!")
create_property(test_container, "k3d::vector3", k3d.vector3(1, 2, 3))

path = testing.binary_path() + "/document.serialization.properties.output.k3d"
document.save(path)

def test_property(value, expected_value):
	if value != expected_value:
		raise "property value doesn't match: ", value, expected_value

document2 = k3d.open_document(path)
test_container2 = document.get_node("test_container")

test_property(test_container2.get_property("k3d::bool_t").internal_value(), True)
test_property(test_container2.get_property("k3d::color").internal_value(), k3d.color(1, 2, 3))
test_property(test_container2.get_property("k3d::double_t").internal_value(), 3.1415)
test_property(test_container2.get_property("k3d::filesystem::path").internal_value(), k3d.generic_path("/foo/bar/baz"))
test_property(test_container2.get_property("k3d::gl::imesh_painter*").internal_value().name, "test_gl_mesh_painter")
test_property(test_container2.get_property("k3d::imaterial*").internal_value().name, "test_material")
test_property(test_container2.get_property("k3d::inode*").internal_value().name, "test_node")
test_property(test_container2.get_property("k3d::int32_t").internal_value(), 19700827)
test_property(test_container2.get_property("k3d::matrix4").internal_value(), k3d.scale3(1, 2, 3))
test_property(test_container2.get_property("k3d::normal3").internal_value(), k3d.normal3(1, 2, 3))
test_property(test_container2.get_property("k3d::point3").internal_value(), k3d.point3(1, 2, 3))
test_property(test_container2.get_property("k3d::point4").internal_value(), k3d.point4(1, 2, 3, 4))
test_property(test_container2.get_property("k3d::ri::imesh_painter*").internal_value().name, "test_ri_mesh_painter")
test_property(test_container2.get_property("k3d::ri::itexture*").internal_value().name, "test_ri_texture")
test_property(test_container2.get_property("k3d::string_t").internal_value(), "K-3D Rocks!")
test_property(test_container2.get_property("k3d::vector3").internal_value(), k3d.vector3(1, 2, 3))

