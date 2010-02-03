PROJECT(k3dsdk-tests)

INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR})
INCLUDE_DIRECTORIES(${k3dsdk_BINARY_DIR})
INCLUDE_DIRECTORIES(${K3D_BOOST_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_SIGC_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

LINK_DIRECTORIES(${K3D_BOOST_LIB_DIRS})
LINK_DIRECTORIES(${K3D_SIGC_LIB_DIRS})

LINK_LIBRARIES(k3dsdk)

ADD_EXECUTABLE(test-difference difference.cpp)
ADD_TEST(k3dsdk.difference ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-difference)

ADD_EXECUTABLE(test-array-metadata array_metadata.cpp)
ADD_TEST(k3dsdk.array.metadata ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-array-metadata)

IF(WIN32 AND K3D_COMPILER_GCC)
	# For some reason, building with optimizations enabled causes link problems with half::eLut and auto-import
	SET_SOURCE_FILES_PROPERTIES(bitmap_conversion.cpp PROPERTIES COMPILE_FLAGS -O0)
ENDIF(WIN32 AND K3D_COMPILER_GCC)
ADD_EXECUTABLE(test-bitmap-conversion bitmap_conversion.cpp)
ADD_TEST(k3dsdk.bitmap.conversion ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-bitmap-conversion)

ADD_EXECUTABLE(test-circular-signals circular_signals.cpp)
ADD_TEST(k3dsdk.circular-signals ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-circular-signals)

ADD_EXECUTABLE(test-hint-mapping hint_mapping.cpp)
ADD_TEST(k3dsdk.hint-mapping ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-hint-mapping)

ADD_EXECUTABLE(test-data-sizes data_sizes.cpp)
ADD_TEST(k3dsdk.data-sizes ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-data-sizes)

ADD_EXECUTABLE(test-float-to-string float_to_string.cpp)
ADD_TEST(k3dsdk.float-to-string.001 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-float-to-string 123)
ADD_TEST(k3dsdk.float-to-string.002 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-float-to-string 123.4)
ADD_TEST(k3dsdk.float-to-string.003 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-float-to-string 123.456)
ADD_TEST(k3dsdk.float-to-string.004 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-float-to-string 123.456789012)
ADD_TEST(k3dsdk.float-to-string.005 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-float-to-string 123.4567890123456)

ADD_EXECUTABLE(test-path-decomposition path_decomposition.cpp)
ADD_TEST(k3dsdk.path.decomposition ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-path-decomposition)

ADD_EXECUTABLE(test-path-relative relative_path.cpp)
ADD_TEST(k3dsdk.path.relative.001 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-path-relative "/home/bubba/k3d/test.k3d" "/" "home/bubba/k3d/test.k3d")
ADD_TEST(k3dsdk.path.relative.002 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-path-relative "/home/bubba/k3d/test.k3d" "/home/bubba" "k3d/test.k3d")
ADD_TEST(k3dsdk.path.relative.003 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-path-relative "/home/bubba/k3d/test.k3d" "/var/documents" "../../home/bubba/k3d/test.k3d")

ADD_EXECUTABLE(test-selection-equality selection_equality.cpp)
ADD_TEST(k3dsdk.selection-equality ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-selection-equality)

ADD_EXECUTABLE(test-selection-serialization selection_serialization.cpp)
ADD_TEST(k3dsdk.selection-serialization ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-selection-serialization)

ADD_EXECUTABLE(test-xml-sanity-checks xml_sanity_checks.cpp)
ADD_TEST(k3dsdk.xml-sanity-checks ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-xml-sanity-checks)

ADD_EXECUTABLE(test-xml-xpath xml_xpath.cpp)
ADD_TEST(k3dsdk.xml-xpath ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-xml-xpath)

ADD_EXECUTABLE(test-document-upgrade document_upgrade.cpp)
ADD_TEST(k3dsdk.document-upgrade ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-document-upgrade)

ADD_EXECUTABLE(test-pipeline-data pipeline_data.cpp)
ADD_TEST(k3dsdk.pipeline-data ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-pipeline-data)

IF(UNIX) 
	ADD_LIBRARY(shared-dynamic-cast SHARED shared_dynamic_cast_library.cpp)
	GET_TARGET_PROPERTY(SHARED_DYNAMIC_CAST_LIBRARY shared-dynamic-cast LOCATION)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/shared_dynamic_cast_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/shared_dynamic_cast_config.h)
	ADD_EXECUTABLE(test-shared-dynamic-cast shared_dynamic_cast.cpp)
	SET_TARGET_PROPERTIES(test-shared-dynamic-cast PROPERTIES LINK_FLAGS -Wl,-E)
	ADD_TEST(k3dsdk.shared-dynamic-cast ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-shared-dynamic-cast)
ENDIF()
