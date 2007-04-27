#python
import k3d

a = "context 1"
k3d.ui.message(a)

recursive_script = """#python
import k3d
a = "context 2"
k3d.ui.message(a)
"""

k3d.execute_script(recursive_script)
k3d.ui.message(a)
