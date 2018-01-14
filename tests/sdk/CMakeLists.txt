INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR})
INCLUDE_DIRECTORIES(${k3dsdk_BINARY_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_SIGC_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_GLIBMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

LINK_DIRECTORIES(${K3D_SIGC_LIB_DIRS})

LINK_LIBRARIES(k3dsdk)

ADD_EXECUTABLE(test-difference difference.cpp)
K3D_TEST(sdk.difference TARGET test-difference LABELS sdk)

ADD_EXECUTABLE(test-array-metadata array_metadata.cpp)
K3D_TEST(sdk.array.metadata TARGET test-array-metadata LABELS sdk)

IF(WIN32 AND K3D_COMPILER_GCC)
	# For some reason, building with optimizations enabled causes link problems with half::eLut and auto-import
	SET_SOURCE_FILES_PROPERTIES(bitmap_conversion.cpp PROPERTIES COMPILE_FLAGS -O0)
ENDIF(WIN32 AND K3D_COMPILER_GCC)
ADD_EXECUTABLE(test-bitmap-conversion bitmap_conversion.cpp)
K3D_TEST(sdk.bitmap.conversion TARGET test-bitmap-conversion LABELS sdk)

ADD_EXECUTABLE(test-circular-signals circular_signals.cpp)
K3D_TEST(sdk.circular-signals TARGET test-circular-signals LABELS sdk)

ADD_EXECUTABLE(test-hint-mapping hint_mapping.cpp)
K3D_TEST(sdk.hint-mapping TARGET test-hint-mapping LABELS sdk)

ADD_EXECUTABLE(test-data-sizes data_sizes.cpp)
K3D_TEST(sdk.data-sizes TARGET test-data-sizes LABELS sdk)

ADD_EXECUTABLE(test-float-to-string float_to_string.cpp)
K3D_TEST(sdk.float-to-string.001 TARGET test-float-to-string ARGUMENTS 123 LABELS sdk)
K3D_TEST(sdk.float-to-string.002 TARGET test-float-to-string ARGUMENTS 123.4 LABELS sdk)
K3D_TEST(sdk.float-to-string.003 TARGET test-float-to-string ARGUMENTS 123.456 LABELS sdk)
K3D_TEST(sdk.float-to-string.004 TARGET test-float-to-string ARGUMENTS 123.456789012 LABELS sdk)
K3D_TEST(sdk.float-to-string.005 TARGET test-float-to-string ARGUMENTS 123.4567890123456 LABELS sdk)

ADD_EXECUTABLE(test-path-decomposition path_decomposition.cpp)
K3D_TEST(sdk.path.decomposition TARGET test-path-decomposition LABELS sdk)

ADD_EXECUTABLE(test-path-relative relative_path.cpp)
K3D_TEST(sdk.path.relative.001 TARGET test-path-relative ARGUMENTS "/home/bubba/k3d/test.k3d" "/" "home/bubba/k3d/test.k3d" LABELS sdk)
K3D_TEST(sdk.path.relative.002 TARGET test-path-relative ARGUMENTS "/home/bubba/k3d/test.k3d" "/home/bubba" "k3d/test.k3d" LABELS sdk)
K3D_TEST(sdk.path.relative.003 TARGET test-path-relative ARGUMENTS "/home/bubba/k3d/test.k3d" "/var/documents" "../../home/bubba/k3d/test.k3d" LABELS sdk)

ADD_EXECUTABLE(test-program-options program_options.cpp)
K3D_TEST(sdk.program-options TARGET test-program-options LABELS sdk)

ADD_EXECUTABLE(test-selection-equality selection_equality.cpp)
K3D_TEST(sdk.selection-equality TARGET test-selection-equality LABELS sdk)

ADD_EXECUTABLE(test-selection-serialization selection_serialization.cpp)
K3D_TEST(sdk.selection-serialization TARGET test-selection-serialization LABELS sdk)

ADD_EXECUTABLE(test-xml-sanity-checks xml_sanity_checks.cpp)
K3D_TEST(sdk.xml-sanity-checks TARGET test-xml-sanity-checks LABELS sdk)

ADD_EXECUTABLE(test-xml-xpath xml_xpath.cpp)
K3D_TEST(sdk.xml-xpath TARGET test-xml-xpath LABELS sdk)

ADD_EXECUTABLE(test-document-upgrade document_upgrade.cpp)
K3D_TEST(sdk.document-upgrade TARGET test-document-upgrade LABELS sdk)

ADD_EXECUTABLE(test-pipeline-data pipeline_data.cpp)
K3D_TEST(sdk.pipeline-data TARGET test-pipeline-data LABELS sdk)

ADD_EXECUTABLE(test-uuid uuid.cpp)
K3D_TEST(sdk.uuid TARGET test-uuid LABELS sdk)

IF(UNIX)
	ADD_LIBRARY(shared-dynamic-cast SHARED shared_dynamic_cast_library.cpp)
	GET_TARGET_PROPERTY(SHARED_DYNAMIC_CAST_LIBRARY shared-dynamic-cast LOCATION)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/shared_dynamic_cast_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/shared_dynamic_cast_config.h)
	ADD_EXECUTABLE(test-shared-dynamic-cast shared_dynamic_cast.cpp)
	target_link_libraries(test-shared-dynamic-cast ${CMAKE_DL_LIBS})
	IF(NOT APPLE)
		SET_TARGET_PROPERTIES(test-shared-dynamic-cast PROPERTIES LINK_FLAGS -Wl,-E)
	ENDIF()
	K3D_TEST(sdk.shared-dynamic-cast TARGET test-shared-dynamic-cast LABELS sdk)
ENDIF()

