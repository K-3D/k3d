PROJECT(tests)

INCLUDE(K3DParseArguments)

# Provides a consistent interface for running all K-3D tests
FUNCTION(K3D_TEST TEST_NAME)
	K3D_PARSE_ARGUMENTS(TEST "TARGET;CMAKE_SCRIPT;PYTHON_SCRIPT;K3D_PYTHON_SCRIPT;REQUIRES;LABELS;WORKING_DIRECTORY;ARGUMENTS" "NGUI;XFAIL" ${ARGN})

	FOREACH(REQUIREMENT ${TEST_REQUIRES})
		IF(NOT ${REQUIREMENT})
			RETURN()
		ENDIF()
	ENDFOREACH()

	IF(TEST_TARGET)
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
		ADD_TEST(${TEST_NAME} "${PYTHON_COMMAND}" "${TEST_PYTHON_SCRIPT}")
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
			--script=${TEST_K3D_PYTHON_SCRIPT}
			--setenv "PYTHONPATH=${tests_SOURCE_DIR}"
			--setenv "K3D_TEST_SOURCE_PATH=${tests_SOURCE_DIR}"
			--setenv "K3D_TEST_BINARY_PATH=${tests_BINARY_DIR}"
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

MACRO(K3D_ADD_TEST TEST_NAME)
	K3D_PARSE_ARGUMENTS(TEST "REQUIRES;LABELS" "NGUI;SCRIPT;PYTHON;XFAIL" ${ARGN})

	SET(REQUIREMENTS_MET 1)
	SET(USER_INTERFACE --ui=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins/k3d-nui.module)
	SET(WILL_FAIL 0)

	IF(TEST_NGUI)
		IF(NOT K3D_BUILD_NGUI_MODULE)
			SET(REQUIREMENTS_MET 0)
		ENDIF()
		SET(USER_INTERFACE --ui=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins/k3d-ngui.module --batch --no-custom-layouts --no-splash)
	ENDIF()

	IF(TEST_SCRIPT)
		SET(SCRIPT_NAME ${TEST_NAME})
		SET(SCRIPT "--script=${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT_NAME}")
	ENDIF()

	IF(TEST_PYTHON)
		IF(NOT K3D_BUILD_PYTHON_MODULE)
			SET(REQUIREMENTS_MET 0)
		ENDIF()
		SET(SCRIPT_NAME ${TEST_NAME})
		SET(SCRIPT "--script=${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT_NAME}")
		SET(PYTHONPATH --setenv "PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}")
	ENDIF()

	FOREACH(REQUIREMENT ${TEST_REQUIRES})
		IF(NOT ${REQUIREMENT})
			SET(REQUIREMENTS_MET 0)
		ENDIF()
	ENDFOREACH()

	IF(REQUIREMENTS_MET)
		ADD_TEST(${TEST_NAME}
			${k3d_BINARY_DIR}/bin/k3d
			--log-level=debug
			--plugins=${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins
			--share=${k3d_SOURCE_DIR}/share
			${USER_INTERFACE}
			${SCRIPT}
			${PYTHONPATH}
			--setenv "K3D_TEST_SOURCE_PATH=${CMAKE_CURRENT_SOURCE_DIR}"
			--setenv "K3D_TEST_BINARY_PATH=${CMAKE_CURRENT_BINARY_DIR}"
			--exit
			)

		IF(TEST_LABELS)
			SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES LABELS ${TEST_LABELS})
		ENDIF()

		IF(TEST_XFAIL)
			SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES WILL_FAIL TRUE)
		ENDIF()
	ENDIF(REQUIREMENTS_MET)
ENDMACRO(K3D_ADD_TEST)

FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bitmaps)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bitmaps/differences)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/meshes)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/meshes/differences)

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

# Run tests that do a simple "smoke-check" on binaries ...
K3D_TEST(startup.k3d-uuidgen TARGET k3d-uuidgen LABELS startup uuid)

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

# Run tests that exercise parallel computation ...
ADD_SUBDIRECTORY(parallel)

# Run tests that exercise offscreen rendering ...
ADD_SUBDIRECTORY(offscreen)

# Run tests that exercise the standard user interface ...
ADD_SUBDIRECTORY(ngui)

#############################################################################333
# Tests waiting to be reorganized ...

K3D_ADD_TEST(document.create.py PYTHON)
K3D_ADD_TEST(document.stress.high_node_count.py PYTHON)
K3D_ADD_TEST(document.stress.all_node_types.py PYTHON)
#K3D_ADD_TEST(document.stress.read_all_properties.py PYTHON)

K3D_ADD_TEST(document.serialization.properties.py PYTHON)

K3D_ADD_TEST(document.importer.bogus_input.py PYTHON)

K3D_ADD_TEST(anim.AnimationTrack.py PYTHON REQUIRES K3D_BUILD_ANIMATION_MODULE)

K3D_ADD_TEST(notifier.InotifyFileChangeNotifier.py PYTHON REQUIRES K3D_BUILD_INOTIFY_MODULE)

K3D_ADD_TEST(benchmark.CompareResults.py PYTHON REQUIRES K3D_BUILD_CUDA_MODULE)

