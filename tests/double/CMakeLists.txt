K3D_TEST(double.source.DoubleSourceScript
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/source.DoubleSourceScript.py
	REQUIRES K3D_BUILD_SCRIPTING_MODULE
	LABELS double source DoubleSourceScript)

K3D_TEST(double.source.PapagayoLipsyncReader
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/source.DoubleSourceScript.py
	REQUIRES K3D_BUILD_LIPSYNC_MODULE
	LABELS double source PapagayoLipsyncReader)

K3D_TEST(double.source.DoubleExpression 
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/source.DoubleExpression.py
	REQUIRES K3D_BUILD_DOUBLE_MODULE
	LABELS double source DoubleExpression)

