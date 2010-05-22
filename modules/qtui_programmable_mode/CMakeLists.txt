QT4_WRAP_CPP(MOC_BUILT_SOURCES mode.h)
K3D_COMPILE_RESOURCE(K3D_RESOURCES default_script.js "/QTUIProgrammableMode/default_script.js")

K3D_BUILD_QTUI_MODULE(k3d-qtui-programmable-mode EXTRA_SOURCES ${MOC_BUILT_SOURCES} ${K3D_RESOURCES})
K3D_CREATE_MODULE_PROXY(k3d-qtui-programmable-mode)
TARGET_LINK_LIBRARIES(k3d-qtui-programmable-mode k3dsdk-qtui-script)
TARGET_LINK_LIBRARIES(k3d-qtui-programmable-mode k3dsdk-resource)

