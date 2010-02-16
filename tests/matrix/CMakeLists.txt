K3D_TEST(matrix.modifier.FrozenTransformation
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.FrozenTransformation.py
	REQUIRES K3D_BUILD_CORE_MODULE
	LABELS matrix modifier FrozenTransformation)

K3D_TEST(matrix.modifier.InvertMatrix
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/modifier.InvertMatrix.py
	REQUIRES K3D_BUILD_MATRIX_MODULE
	LABELS matrix modifier InvertMatrix)

