# Build the module
K3D_BUILD_NGUI_MODULE(k3d-ngui-aqsis)
K3D_CREATE_MODULE_PROXY(k3d-ngui-aqsis)
TARGET_LINK_LIBRARIES(k3d-ngui-aqsis aqsis_core)
