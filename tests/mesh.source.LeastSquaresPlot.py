#python

import k3d
import testing

document = k3d.new_document()

source = document.new_node("LeastSquaresPlot")

source.create_property("k3d::string_t","function_1", "Function 1", "Function 1")
source.create_property("k3d::string_t","function_2", "Function 2", "Function 2")
source.create_property("k3d::string_t","function_3", "Function 3", "Function 3")

source.function_1 = "u^2"
source.function_2 = "sin(v)"
source.function_3 = "u*v"

input_points = document.new_node("FrozenMesh")

mesh = k3d.dynamic_cast(input_points, "imesh_storage").reset_mesh()

positions = [(-5, 1, 3), (4, 2, 4), (3, 3, -5), (-4, 4, -3)]

points = mesh.create_points()
point_selection = mesh.create_point_selection()

for position in positions:
	points.append(k3d.point3(position[0], position[1], position[2]))
	point_selection.append(0.0)

document.set_dependency(source.get_property("input_mesh"), input_points.get_property("output_mesh"))

testing.mesh_comparison_to_reference(document, source.get_property("output_mesh"), "mesh.source.LeastSquaresPlot", 2048)

