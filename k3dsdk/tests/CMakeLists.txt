PROJECT(k3dsdk-tests)

INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR})
INCLUDE_DIRECTORIES(${k3dsdk_BINARY_DIR})
INCLUDE_DIRECTORIES(${K3D_BOOST_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_SIGC_INCLUDE_DIRS})

LINK_DIRECTORIES(${K3D_BOOST_LIB_DIRS})
LINK_DIRECTORIES(${K3D_SIGC_LIB_DIRS})

LINK_LIBRARIES(k3dsdk)

ADD_EXECUTABLE(test-almost-equal almost_equal.cpp)
ADD_TEST(k3dsdk.equal.almost ${EXECUTABLE_OUTPUT_PATH}/test-almost-equal)

ADD_EXECUTABLE(test-array-metadata array_metadata.cpp)
ADD_TEST(k3dsdk.array.metadata ${EXECUTABLE_OUTPUT_PATH}/test-array-metadata)

IF(WIN32 AND K3D_COMPILER_GCC)
	# For some reason, building with optimizations enabled causes link problems with half::eLut and auto-import
	SET_SOURCE_FILES_PROPERTIES(bitmap_conversion.cpp PROPERTIES COMPILE_FLAGS -O0)
ENDIF(WIN32 AND K3D_COMPILER_GCC)
ADD_EXECUTABLE(test-bitmap-conversion bitmap_conversion.cpp)
ADD_TEST(k3dsdk.bitmap.conversion ${EXECUTABLE_OUTPUT_PATH}/test-bitmap-conversion)

ADD_EXECUTABLE(test-circular-signals circular_signals.cpp)
ADD_TEST(k3dsdk.circular-signals ${EXECUTABLE_OUTPUT_PATH}/test-circular-signals)

ADD_EXECUTABLE(test-hint-mapping hint_mapping.cpp)
ADD_TEST(k3dsdk.hint-mapping ${EXECUTABLE_OUTPUT_PATH}/test-hint-mapping)

ADD_EXECUTABLE(test-data-sizes data_sizes.cpp)
ADD_TEST(k3dsdk.data-sizes ${EXECUTABLE_OUTPUT_PATH}/test-data-sizes)

ADD_EXECUTABLE(test-float-to-string float_to_string.cpp)
ADD_TEST(k3dsdk.float-to-string.001 ${EXECUTABLE_OUTPUT_PATH}/test-float-to-string 123)
ADD_TEST(k3dsdk.float-to-string.002 ${EXECUTABLE_OUTPUT_PATH}/test-float-to-string 123.4)
ADD_TEST(k3dsdk.float-to-string.003 ${EXECUTABLE_OUTPUT_PATH}/test-float-to-string 123.456)
ADD_TEST(k3dsdk.float-to-string.004 ${EXECUTABLE_OUTPUT_PATH}/test-float-to-string 123.456789012)
ADD_TEST(k3dsdk.float-to-string.005 ${EXECUTABLE_OUTPUT_PATH}/test-float-to-string 123.4567890123456)

ADD_EXECUTABLE(test-mesh-selection mesh_selection.cpp)
ADD_TEST(k3dsdk.mesh_selection.001 ${EXECUTABLE_OUTPUT_PATH}/test-mesh-selection "[0, 1) 1" 0 1 1)
ADD_TEST(k3dsdk.mesh_selection.002 ${EXECUTABLE_OUTPUT_PATH}/test-mesh-selection "[0, 1) 1 [1, 2) 2" 0 1 1 1 2 2)
ADD_TEST(k3dsdk.mesh_selection.003 ${EXECUTABLE_OUTPUT_PATH}/test-mesh-selection "[0, 1) 1 [1, 2) 2 [2, 3) 3" 0 1 1 1 2 2 2 3 3)
ADD_TEST(k3dsdk.mesh_selection.004 ${EXECUTABLE_OUTPUT_PATH}/test-mesh-selection "[0, 1) 1 [0, 1) 1" 0 1 1 0 1 1)

ADD_EXECUTABLE(test-mesh-sizes mesh_sizes.cpp)
ADD_TEST(k3dsdk.mesh-sizes ${EXECUTABLE_OUTPUT_PATH}/test-mesh-sizes)

ADD_EXECUTABLE(test-path-decomposition path_decomposition.cpp)
ADD_TEST(k3dsdk.path.decomposition ${EXECUTABLE_OUTPUT_PATH}/test-path-decomposition)

ADD_EXECUTABLE(test-path-relative relative_path.cpp)
ADD_TEST(k3dsdk.path.relative.001 ${EXECUTABLE_OUTPUT_PATH}/test-path-relative "/home/bubba/k3d/test.k3d" "/" "home/bubba/k3d/test.k3d")
ADD_TEST(k3dsdk.path.relative.002 ${EXECUTABLE_OUTPUT_PATH}/test-path-relative "/home/bubba/k3d/test.k3d" "/home/bubba" "k3d/test.k3d")
ADD_TEST(k3dsdk.path.relative.003 ${EXECUTABLE_OUTPUT_PATH}/test-path-relative "/home/bubba/k3d/test.k3d" "/var/documents" "../../home/bubba/k3d/test.k3d")

ADD_EXECUTABLE(test-selection-serialization selection_serialization.cpp)
ADD_TEST(k3dsdk.selection-serialization ${EXECUTABLE_OUTPUT_PATH}/test-selection-serialization)

ADD_EXECUTABLE(test-xml-sanity-checks xml_sanity_checks.cpp)
ADD_TEST(k3dsdk.xml-sanity-checks ${EXECUTABLE_OUTPUT_PATH}/test-xml-sanity-checks)

ADD_EXECUTABLE(test-pipeline-data pipeline_data.cpp)
ADD_TEST(k3dsdk.pipeline-data ${EXECUTABLE_OUTPUT_PATH}/test-pipeline-data)

