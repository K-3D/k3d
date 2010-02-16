#python

import k3d
import testing

doc = k3d.new_document()
source = doc.new_node("DoubleSourceScript")

if source.output_double != 1.0:
	raise "unexpected output_double: " + str(source.output_double)

