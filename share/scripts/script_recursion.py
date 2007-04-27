#python
import k3d

k3d.ui.message("Howdy,")

recursive_script = """#python
import k3d
k3d.ui.message("World!")
"""

k3d.execute_script(recursive_script)
