#python

import testing
import k3d
from math import pi

setup = testing.setup_mesh_modifier_test("NurbsCircle","NurbsRevolveCurve")

setup.source.thetamax = pi
setup.modifier.angle = 0.5*pi
setup.modifier.around = 'x'

setup.modifier.mesh_selection = k3d.geometry.selection.create(1)

testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.NurbsRevolveCurve", 2, ["Darwin-i386"])

