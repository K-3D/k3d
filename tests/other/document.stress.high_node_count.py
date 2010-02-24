#python

import k3d

doc = k3d.new_document();
for i in range(0, 10000):
	node = doc.new_node("FrozenMesh");
	node.name = "Node " + str(i);

