# Get the set(s) of available shaders ...
FILE(GLOB_RECURSE SURFACE_SHADER_LIST ${share_SOURCE_DIR}/shaders/surface/*.sl)
LIST(SORT SURFACE_SHADER_LIST)

FILE(GLOB_RECURSE DISPLACEMENT_SHADER_LIST ${share_SOURCE_DIR}/shaders/displacement/*.sl)
LIST(SORT DISPLACEMENT_SHADER_LIST)

FILE(GLOB_RECURSE LIGHT_SHADER_LIST ${share_SOURCE_DIR}/shaders/light/*.sl)
LIST(SORT LIGHT_SHADER_LIST)

FILE(GLOB_RECURSE SHADER_LIST ${share_SOURCE_DIR}/shaders/*.sl)
LIST(SORT SHADER_LIST)

# Figure-out which render engines we have available ...
FIND_PROGRAM(AQSL_COMMAND aqsl)
FIND_PROGRAM(AQSIS_COMMAND aqsis)
FIND_PROGRAM(RNDR_COMMAND rndr)
FIND_PROGRAM(SDRC_COMMAND sdrc)
MARK_AS_ADVANCED(AQSL_COMMAND)
MARK_AS_ADVANCED(AQSIS_COMMAND)
MARK_AS_ADVANCED(RNDR_COMMAND)
MARK_AS_ADVANCED(SDRC_COMMAND)

# Test shader compilation with Aqsis ...
IF(AQSL_COMMAND)
	FOREACH(SHADER ${SHADER_LIST})
		GET_FILENAME_COMPONENT(SHADER_NAME ${SHADER} NAME_WE)
		GET_FILENAME_COMPONENT(SHADER_PATH ${SHADER} PATH)
		K3D_TEST(shader.compile.${SHADER_NAME}.aqsis
			COMMAND ${AQSL_COMMAND} -I "${SHADER_PATH}" -I "${share_SOURCE_DIR}/shaders" -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.slx ${SHADER}
			LABELS shader aqsis)
	ENDFOREACH()
ENDIF()

# Test shader compilation with Pixie ...
IF(SDRC_COMMAND)
	FOREACH(SHADER ${SHADER_LIST})
		GET_FILENAME_COMPONENT(SHADER_NAME ${SHADER} NAME_WE)
		GET_FILENAME_COMPONENT(SHADER_PATH ${SHADER} PATH)
		K3D_TEST(shader.compile.${SHADER_NAME}.pixie
			COMMAND ${SDRC_COMMAND} "-I${SHADER_PATH}" "-I${share_SOURCE_DIR}/shaders" -o ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.sdr ${SHADER}
			LABELS shader pixie)
	ENDFOREACH()
ENDIF()

# Generate sample shader images with Aqsis ...
IF(AQSIS_COMMAND)
	FOREACH(SHADER ${DISPLACEMENT_SHADER_LIST})
	GET_FILENAME_COMPONENT(SHADER_NAME ${SHADER} NAME_WE)
	K3D_TEST(shader.displacement.render.${SHADER_NAME}.aqsis
		K3D_PYTHON_COMMAND_SCRIPT "#python\nimport testing\ntesting.displacement_shader_test(\"Aqsis\", \"${SHADER_NAME}\")"
		LABELS shader displacement aqsis)
	ENDFOREACH()

	FOREACH(SHADER ${LIGHT_SHADER_LIST})
	GET_FILENAME_COMPONENT(SHADER_NAME ${SHADER} NAME_WE)
	K3D_TEST(shader.light.render.${SHADER_NAME}.aqsis
		K3D_PYTHON_COMMAND_SCRIPT "#python\nimport testing\ntesting.light_shader_test(\"Aqsis\", \"${SHADER_NAME}\")"
		LABELS shader light aqsis)
	ENDFOREACH()

	FOREACH(SHADER ${SURFACE_SHADER_LIST})
	GET_FILENAME_COMPONENT(SHADER_NAME ${SHADER} NAME_WE)
	K3D_TEST(shader.surface.render.${SHADER_NAME}.aqsis
		K3D_PYTHON_COMMAND_SCRIPT "#python\nimport testing\ntesting.surface_shader_test(\"Aqsis\", \"${SHADER_NAME}\")"
		LABELS shader surface aqsis)
	ENDFOREACH()
ENDIF()

# Generate sample shader images with Pixie ...
IF(RNDR_COMMAND)
	FOREACH(SHADER ${DISPLACEMENT_SHADER_LIST})
	GET_FILENAME_COMPONENT(SHADER_NAME ${SHADER} NAME_WE)
	K3D_TEST(shader.displacement.render.${SHADER_NAME}.pixie
		K3D_PYTHON_COMMAND_SCRIPT "#python\nimport testing\ntesting.displacement_shader_test(\"Pixie\", \"${SHADER_NAME}\")"
		LABELS shader displacement pixie)
	ENDFOREACH()

	FOREACH(SHADER ${LIGHT_SHADER_LIST})
	GET_FILENAME_COMPONENT(SHADER_NAME ${SHADER} NAME_WE)
	K3D_TEST(shader.light.render.${SHADER_NAME}.pixie
		K3D_PYTHON_COMMAND_SCRIPT "#python\nimport testing\ntesting.light_shader_test(\"Pixie\", \"${SHADER_NAME}\")"
		LABELS shader light pixie)
	ENDFOREACH()

	FOREACH(SHADER ${SURFACE_SHADER_LIST})
	GET_FILENAME_COMPONENT(SHADER_NAME ${SHADER} NAME_WE)
	K3D_TEST(shader.surface.render.${SHADER_NAME}.pixie
		K3D_PYTHON_COMMAND_SCRIPT "#python\nimport testing\ntesting.surface_shader_test(\"Pixie\", \"${SHADER_NAME}\")"
		LABELS shader surface pixie)
	ENDFOREACH()
ENDIF()

