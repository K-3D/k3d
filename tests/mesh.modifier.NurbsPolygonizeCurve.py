#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsPolygonizeCurve")

selection = k3d.mesh_selection.deselect_all()
selection.nurbs_curves = k3d.mesh_selection.component_select_all()

setup.modifier.mesh_selection = selection
setup.modifier.segments = 10
setup.modifier.delete_original = True

testing.mesh_comparison_to_reference(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsPolygonizeCurve", 2)

