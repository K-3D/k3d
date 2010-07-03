#python
import k3d

a = "context 1"
k3d.ui().message(a)

recursive_script = """#python
import k3d
a = "context 2"
k3d.ui().message(a)
"""

k3d.script.execute(recursive_script)
k3d.ui().message(a)

