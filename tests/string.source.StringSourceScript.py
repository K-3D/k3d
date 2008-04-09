#python

import k3d
import testing

doc = k3d.new_document()
source = doc.new_node("StringSourceScript")

if source.output_string != "K-3D":
	raise "unexpected output_string: " + source.output_string

