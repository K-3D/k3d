#python

import k3d
import testing
import copy

source_file = "papagayo_example.dat"

setup = testing.setup_scalar_source_test("PapagayoLipsyncReader")

setup.source.frame_rate = 30
setup.source.interpolate = True
setup.source.interpolation_time = 0.2

setup.source.papagayo_file = k3d.filesystem.generic_path(testing.source_path() + "/lipsync/" + source_file)

test_cases = \
[
[0.0,{"rest":1.0}],
[1.98,{"E":0.24000000000000021,"etc":0.75999999999999979}],
[2.0,{"E":0.5,"etc":0.5}],
[4.34,{"E":0.69999999999999463,"MBP":0.30000000000000537}],
]

mouths = ["AI","E","etc","FV","L","MBP","O","rest","U","WQ"]

for test_case in test_cases:
  setup.source.time = test_case[0]
  source_mouth_value = 0.0
  mouths_in_zero = copy.deepcopy(mouths)
  for mouth,reference_value in test_case[1].iteritems():
    exec("source_mouth_value = setup.source."+mouth)
    testing.scalar_comparison(source_mouth_value,reference_value)
    mouths_in_zero.remove(mouth)
  #Check the other mouths are in zero
  for mouth in mouths_in_zero:
    exec("source_mouth_value = setup.source."+mouth)
    testing.scalar_comparison(source_mouth_value,0.0)
