#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "CUDATransformPoints")

selection = k3d.geometry.selection.create(0)
selection.points = k3d.geometry.point_selection.uniform(selection, 1)

setup.modifier.mesh_selection = selection
setup.modifier.input_matrix = k3d.translate3(k3d.vector3(0, 0, 1))

testing.mesh_reference_comparison(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.TransformPoints", 1)
