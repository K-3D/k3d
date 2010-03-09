#python

import k3d
import testing

document = k3d.new_document()

small_cube = k3d.plugin.create("PolyCube", document)
small_cube.width = 2.5
small_cube.depth = 2.5
small_cube.height = 7.5
big_cube = k3d.plugin.create("PolyCube", document)

cgal_boolean = k3d.plugin.create("CGALBoolean", document)
cgal_boolean.type = "difference"
cgal_boolean.create_property("k3d::mesh*", "input_1", "Input 1", "")
cgal_boolean.create_property("k3d::mesh*", "input_2", "Input 2", "")

document.set_dependency(cgal_boolean.get_property("input_1"), big_cube.get_property("output_mesh"))
document.set_dependency(cgal_boolean.get_property("input_2"), small_cube.get_property("output_mesh"))


testing.require_valid_mesh(document, cgal_boolean.get_property("output_mesh"))
testing.require_similar_mesh(document, cgal_boolean.get_property("output_mesh"), "mesh.modifier.CGALBoolean.cubes", 1)