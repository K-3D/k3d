#python

import k3d
import testing

doc = k3d.new_document()
source = k3d.plugin.create("ColorSourceScript", doc)

if source.output_color != k3d.color(1, 0.5, 0.25):
	raise "unexpected output_color: " + str(source.output_color)

