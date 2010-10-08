K3D_COMPILE_RESOURCE(RESOURCES completion.py "/module/ngui_python_shell/completion.py")

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

K3D_BUILD_NGUI_MODULE(k3d-ngui-python-shell EXTRA_SOURCES ${RESOURCES})
K3D_CREATE_MODULE_PROXY(k3d-ngui-python-shell)

TARGET_LINK_LIBRARIES(k3d-ngui-python-shell k3dsdk-python)
TARGET_LINK_LIBRARIES(k3d-ngui-python-shell ${PYTHON_LIBRARIES})
