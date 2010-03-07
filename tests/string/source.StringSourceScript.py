#python

import k3d
import testing

doc = k3d.new_document()
source = k3d.plugin.create("StringSourceScript", doc)

if source.output_string != "K-3D":
	raise "unexpected output_string: " + source.output_string

