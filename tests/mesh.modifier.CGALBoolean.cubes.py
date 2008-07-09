#python

import k3d
import testing

document = k3d.new_document()

small_cube = document.new_node("PolyCube")
small_cube.width = 2.5
small_cube.depth = 2.5
small_cube.height = 7.5
big_cube = document.new_node("PolyCube")

cgal_boolean = document.new_node("CGALBoolean")
cgal_boolean.type = "difference"
cgal_boolean.create_property("k3d::mesh*", "input_1", "Input 1", "")
cgal_boolean.create_property("k3d::mesh*", "input_2", "Input 2", "")

document.set_dependency(cgal_boolean.get_property("input_1"), big_cube.get_property("output_mesh"))
document.set_dependency(cgal_boolean.get_property("input_2"), small_cube.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, cgal_boolean.get_property("output_mesh"), "mesh.modifier.CGALBoolean.cubes", 1)
