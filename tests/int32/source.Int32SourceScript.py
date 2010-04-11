#python

import k3d
import testing

document = k3d.new_document()
source = k3d.plugin.create("Int32SourceScript", document)

if source.output_int32 != 1:
	raise "unexpected output_int32: " + str(source.output_int32)

