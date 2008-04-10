#python

import k3d
import testing

doc = k3d.new_document()
source = doc.new_node("ScalarSourceScript")

if source.output_scalar != 1.0:
	raise "unexpected output_scalar: " + str(source.output_scalar)

