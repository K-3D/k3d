#python

import testing
import k3d

document = k3d.new_document()
setup = testing.setup_mesh_modifier_test("NurbsCylinder","NurbsExtractPatchCurve")

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)
setup.modifier.parameter = 0.5
setup.modifier.extract_v_curve = True

testing.mesh_comparison_to_reference(document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsExtractPatchCurve", 1)

