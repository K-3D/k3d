#python

import k3d
import testing

doc = k3d.new_document()
source = doc.new_node("Int32SourceScript")

if source.output_int32 != 1:
	raise "unexpected output_int32: " + str(source.output_int32)

