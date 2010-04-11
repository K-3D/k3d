#python

import k3d
import testing

doc = k3d.new_document()
source = k3d.plugin.create("DoubleSourceScript", doc)

if source.output_double != 1.0:
	raise "unexpected output_double: " + str(source.output_double)

