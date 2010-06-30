PROJECT(k3d-external)

SET(K3D_EXTERNAL_MODULES "" CACHE STRING "Semicolon-separated list of external modules that will be included in the build.")

FOREACH(MODULE ${K3D_EXTERNAL_MODULES})
	OPTION(K3D_BUILD_${MODULE} "Build external ${MODULE} module" ON)

	IF(K3D_BUILD_${MODULE})
		FIND_PATH(${MODULE}_EXTERNAL_SOURCE_DIR
			CMakeLists.txt
			DOC "Path to the ${MODULE} directory."
			)

		SET(MODULE_DIRECTORY ${${MODULE}_EXTERNAL_SOURCE_DIR})

		IF(NOT EXISTS ${MODULE_DIRECTORY})
			MESSAGE(SEND_ERROR "You must specify an existing \"${MODULE}\" source directory using ${MODULE}_EXTERNAL_SOURCE_DIR.")
		ELSEIF(NOT EXISTS ${MODULE_DIRECTORY}/CMakeLists.txt)
			MESSAGE(SEND_ERROR "The \"${MODULE}\" source directory \"${MODULE_DIRECTORY}\" must contain a CMakeLists.txt file.")
		ELSE()
			ADD_SUBDIRECTORY(${MODULE_DIRECTORY} "${CMAKE_CURRENT_BINARY_DIR}/${MODULE}")
		ENDIF()
	ENDIF()
ENDFOREACH()

