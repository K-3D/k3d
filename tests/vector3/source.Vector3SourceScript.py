#python

import k3d
import testing

doc = k3d.new_document()
source = k3d.plugin.create("Vector3SourceScript", doc)

if source.output_vector != k3d.vector3(1, 2, 3):
	raise "unexpected output_vector: " + str(source.output_vector)

