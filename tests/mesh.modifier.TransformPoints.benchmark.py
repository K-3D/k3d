#python

import testing
import k3d

# this function is used to setup the benchmark for the mesh modifiers
def createSimpleCube(document):
    cube = document.new_node("PolyCube")
    cube.columns = 10;
    cube.rows = 10;
    cube.slices = 10;
    return cube

testing.mesh_modifier_benchmark("TransformPoints", createSimpleCube, 10, {"input_matrix" : k3d.translate3(k3d.vector3(0, 0, 1))})
