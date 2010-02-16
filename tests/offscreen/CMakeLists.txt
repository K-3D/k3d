K3D_TEST(offscreen.GLXCameraToBitmap
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/offscreen.GLXCameraToBitmap.py
	REQUIRES K3D_BUILD_GLX_MODULE
	LABELS offscreen GLXCameraToBitmap)

K3D_TEST(offscreen.OpenGLFramebufferCameraToBitmap
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/offscreen.OpenGLFramebufferCameraToBitmap.py
	REQUIRES K3D_BUILD_OPENGL_FRAMEBUFFER_MODULE
	LABELS offscreen OpenGLFramebufferCameraToBitmap)

K3D_TEST(offscreen.VirtualCameraToBitmap
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/offscreen.VirtualCameraToBitmap.py
	REQUIRES K3D_BUILD_VIRTUAL_OFFSCREEN_MODULE
	LABELS offscreen VirtualCameraToBitmap)

K3D_TEST(offscreen.WGLCameraToBitmap
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/offscreen.WGLCameraToBitmap.py
	REQUIRES K3D_BUILD_WGL_MODULE
	LABELS offscreen WGLCameraToBitmap)
