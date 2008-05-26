PROJECT(k3d-external)

SET(K3D_EXTERNAL_MODULES "" CACHE STRING
	"Semicolon-separated list of external modules that will be included in the build.  Each module \"foo\" must provide a \"foo.cmake\" file.")

FOREACH(module ${K3D_EXTERNAL_MODULES})
	OPTION(K3D_BUILD_${module} "Build external ${module} module" ON)

	IF(K3D_BUILD_${module})
		FIND_PATH(${module}_EXTERNAL_SOURCE_DIR
			${module}.cmake
			DOC "Path to the ${module}.cmake file."
			)

		SET(MODULE_DIRECTORY ${${module}_EXTERNAL_SOURCE_DIR})

		IF(EXISTS ${MODULE_DIRECTORY})
			SET(MODULE_MAKEFILE ${MODULE_DIRECTORY}/${module}.cmake)

			IF(EXISTS ${MODULE_MAKEFILE})
				INCLUDE(${MODULE_MAKEFILE})
			ELSE(EXISTS ${MODULE_MAKEFILE})
				MESSAGE(SEND_ERROR "Cannot find required \"${module}\" module makefile \"${MODULE_MAKEFILE}\"")
			ENDIF(EXISTS ${MODULE_MAKEFILE})
		ELSE(EXISTS ${MODULE_DIRECTORY})
			MESSAGE(SEND_ERROR "The \"${module}\" module source directory \"${MODULE_DIRECTORY}\" does not exist.
				Specify a different directory using ${module}_EXTERNAL_SOURCE_DIR")
		ENDIF(EXISTS ${MODULE_DIRECTORY})

	ENDIF(K3D_BUILD_${module})

ENDFOREACH(module)

