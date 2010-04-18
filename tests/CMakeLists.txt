PROJECT(tests)

INCLUDE(K3DParseArguments)

# Provides a consistent interface for running all K-3D tests
FUNCTION(K3D_TEST TEST_NAME)
	K3D_PARSE_ARGUMENTS(TEST "COMMAND;TARGET;CMAKE_SCRIPT;PYTHON_SCRIPT;K3D_PYTHON_SCRIPT;K3D_PYTHON_COMMAND_SCRIPT;REQUIRES;LABELS;WORKING_DIRECTORY;ARGUMENTS" "NGUI;XFAIL" ${ARGN})

	FOREACH(REQUIREMENT ${TEST_REQUIRES})
		IF(NOT ${REQUIREMENT})
			RETURN()
		ENDIF()
	ENDFOREACH()

	IF(TEST_COMMAND)
		ADD_TEST(${TEST_NAME} ${TEST_COMMAND})
	ELSEIF(TEST_TARGET)
		GET_TARGET_PROPERTY(TARGET_LOCATION ${TEST_TARGET} LOCATION)
		IF(TEST_WORKING_DIRECTORY)
			ADD_TEST(${TEST_NAME} "${CMAKE_COMMAND}" -E chdir "${TEST_WORKING_DIRECTORY}" "${TARGET_LOCATION}" ${TEST_ARGUMENTS})
		ELSE()
			ADD_TEST(${TEST_NAME} "${TARGET_LOCATION}" ${TEST_ARGUMENTS})
		ENDIF()
	ELSEIF(TEST_CMAKE_SCRIPT)
		IF(NOT TEST_WORKING_DIRECTORY)
			MESSAGE(SEND_ERROR "CMAKE_SCRIPT directive requires WORKING_DIRECTORY")
		ENDIF()
		ADD_TEST(${TEST_NAME} "${CMAKE_COMMAND}" -E chdir "${TEST_WORKING_DIRECTORY}" "${CMAKE_COMMAND}" -P "${TEST_CMAKE_SCRIPT}")
	ELSEIF(TEST_PYTHON_SCRIPT)
		ADD_TEST(${TEST_NAME} "${PYTHON_EXECUTABLE}" "${TEST_PYTHON_SCRIPT}")
	ELSEIF(TEST_K3D_PYTHON_SCRIPT)
		SET(TEST_UI --ui=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins/k3d-nui.module)
		IF(TEST_NGUI)
			IF(NOT K3D_BUILD_NGUI_MODULE)
				RETURN()
			ENDIF()
			SET(TEST_UI --ui=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins/k3d-ngui.module --batch --no-custom-layouts --no-splash)
		ENDIF()

		ADD_TEST(${TEST_NAME}
			${k3d_BINARY_DIR}/bin/k3d
			--log-level=debug
			--plugins=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins
			--share=${k3d_SOURCE_DIR}/share
			${TEST_UI}
			--setenv "PYTHONPATH=${tests_SOURCE_DIR}"
			--setenv "K3D_TEST_SOURCE_PATH=${tests_SOURCE_DIR}"
			--setenv "K3D_TEST_BINARY_PATH=${tests_BINARY_DIR}"
			${TEST_ARGUMENTS}
			--script-file=${TEST_K3D_PYTHON_SCRIPT}
			--batch
			--exit
			)
	ELSEIF(TEST_K3D_PYTHON_COMMAND_SCRIPT)
		SET(TEST_UI --ui=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins/k3d-nui.module)
		IF(TEST_NGUI)
			IF(NOT K3D_BUILD_NGUI_MODULE)
				RETURN()
			ENDIF()
			SET(TEST_UI --ui=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins/k3d-ngui.module --batch --no-custom-layouts --no-splash)
		ENDIF()

		ADD_TEST(${TEST_NAME}
			${k3d_BINARY_DIR}/bin/k3d
			--log-level=debug
			--plugins=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins
			--share=${k3d_SOURCE_DIR}/share
			${TEST_UI}
			--setenv "PYTHONPATH=${tests_SOURCE_DIR}"
			--setenv "K3D_TEST_SOURCE_PATH=${tests_SOURCE_DIR}"
			--setenv "K3D_TEST_BINARY_PATH=${tests_BINARY_DIR}"
			${TEST_ARGUMENTS}
			--script=${TEST_K3D_PYTHON_COMMAND_SCRIPT}
			--batch
			--exit
			)
	ELSE()
		MESSAGE(SEND_ERROR "Unknown K3D_TEST type")
		RETURN()
	ENDIF()

	IF(TEST_LABELS)
		FOREACH(LABEL ${TEST_LABELS})
			SET(LABELS "${LABELS} ${LABEL}")
		ENDFOREACH()
		SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES LABELS ${LABELS})
	ENDIF()

	IF(TEST_XFAIL)
		SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES WILL_FAIL TRUE)
	ENDIF()
ENDFUNCTION()

FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bitmaps)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bitmaps/differences)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/meshes)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/meshes/differences)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders/displacement)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders/light)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders/surface)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/shaders/volume)

# Run tests that extract information about the underlying platform
ADD_SUBDIRECTORY(platform)

# Run tests that enforce coding-standards
ADD_SUBDIRECTORY(code)

# Run tests that verify the functionality of the testing system itself
ADD_SUBDIRECTORY(testing)

# Run tests that exercise the C++ SDK
ADD_SUBDIRECTORY(sdk)

# Run tests that exercise the Python SDK
ADD_SUBDIRECTORY(python)

# Run tests that exercise double-related functionality
ADD_SUBDIRECTORY(double)

ADD_SUBDIRECTORY(int32)

ADD_SUBDIRECTORY(color)

ADD_SUBDIRECTORY(vector3)

# Run tests that exercise matrix-related functionality
ADD_SUBDIRECTORY(matrix)

ADD_SUBDIRECTORY(string)

# Run tests that exercise bitmap-related functionality
ADD_SUBDIRECTORY(bitmap)

# Run tests that exercise mesh-related functionality
ADD_SUBDIRECTORY(mesh)

# Run tests that exercise shaders ...
ADD_SUBDIRECTORY(shaders)

# Run tests that exercise parallel computation ...
ADD_SUBDIRECTORY(parallel)

# Run tests that exercise offscreen rendering ...
ADD_SUBDIRECTORY(offscreen)

# Run tests that exercise animation ...
ADD_SUBDIRECTORY(animation)

# Run tests that exercise the standard user interface ...
ADD_SUBDIRECTORY(ngui)

# Run miscellaneous odds-and-ends ...
ADD_SUBDIRECTORY(other)

