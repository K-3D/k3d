K3D_TEST(testing.measurements
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/measurements.py
	LABELS testing)

K3D_TEST(testing.detect.language.failure
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/detect_language_failure
	LABELS testing
	XFAIL)

K3D_TEST(testing.detect.python
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/detect_python.py
	LABELS testing)

K3D_TEST(testing.detect.python.failure
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/detect_python_failure.py
	LABELS testing XFAIL)

K3D_TEST(testing.detect.python.exception
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/detect_python_exception.py
	LABELS testing XFAIL)

K3D_TEST(testing.PipelineProfiler
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/PipelineProfiler.py
	LABELS testing)

