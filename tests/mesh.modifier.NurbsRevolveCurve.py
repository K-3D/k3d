#python

import testing
import k3d
from math import pi

document = k3d.new_document()
setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsRevolveCurve")

setup.source.thetamax = pi
setup.modifier.angle = 0.5*pi
setup.modifier.around = 'x'

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)

testing.mesh_comparison_to_reference(document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsRevolveCurve", 1)
