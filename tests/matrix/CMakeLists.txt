K3D_TEST(matrix.modifier.FrozenMatrix
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.FrozenMatrix.py
	REQUIRES K3D_BUILD_CORE_MODULE
	LABELS matrix modifier FrozenMatrix)

K3D_TEST(matrix.modifier.MatrixInverse
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.MatrixInverse.py
	REQUIRES K3D_BUILD_MATRIX_MODULE
	LABELS matrix modifier MatrixInverse)

