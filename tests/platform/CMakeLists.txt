K3D_TEST(platform.configuration PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/platform_configuration.py LABELS platform)

INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR})
INCLUDE_DIRECTORIES(${k3d_BINARY_DIR}/k3dsdk)
INCLUDE_DIRECTORIES(${K3D_BOOST_INCLUDE_DIRS})

ADD_EXECUTABLE(test-platform-types platform_types.cpp)
K3D_TEST(platform.types TARGET test-platform-types LABELS platform)

ADD_EXECUTABLE(test-floating-point platform_floating_point.cpp)
K3D_TEST(platform.float TARGET test-floating-point LABELS platform)

