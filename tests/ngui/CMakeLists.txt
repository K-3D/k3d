K3D_TEST(ngui.MoveTool
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/MoveTool.py
	NGUI
	LABELS ngui tool)

K3D_TEST(ngui.RotateTool
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/RotateTool.py
	NGUI
	LABELS ngui tool)

K3D_TEST(ngui.icons.missing
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/icons.missing.py
	LABELS ngui icons)

K3D_TEST(ngui.icons.unexpected
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/icons.unexpected.py
	LABELS ngui icons)

K3D_TEST(ngui.icons.unrasterized
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/icons.unrasterized.py
	LABELS ngui icons)

