K3D_TEST(parallel.grain_size.ScalePoints
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/grain_size.ScalePoints.py
	REQUIRES K3D_ENABLE_PARALLEL
	LABELS parallel ScalePoints)

K3D_TEST(parallel.thread_count.ScalePoints
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/thread_count.ScalePoints.py
	REQUIRES K3D_ENABLE_PARALLEL
	LABELS paralle ScalePoints)

