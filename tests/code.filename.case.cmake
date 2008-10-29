# Ensure that filenames are all lower-case (excepting third-party sources)

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"^AUTHORS$"
	"^.*CMakeLists.txt$"
	"^cmake/modules/K3D.*$"
	"^COPYING$"
	"^CTestConfig.cmake$"
	"^distribution/osx/bundle/Info.plist.in$"
	"^docs/doxygen/Doxyfile.in$"
	"^INSTALL$"
	"^k3dsdk/ftgl/.*$"
	"^k3dsdk/Half/.*$"
	"^k3dsdk/sgi_tesselator/README$"
	"^modules/cuda/FindCUDA.cmake$"
	"^modules/cloth/d_Vec3f.h$"
	"^modules/cloth/d_Vecf.h$"
	"^modules/ngui_material_manager_panel/.*.h$"
	"^modules/ngui_material_manager_panel/.*.cpp$"
	"^modules/pdiff/CompareArgs.*$"
	"^modules/pdiff/LPyramid.*$"
	"^modules/pdiff/Metric.*$"
	"^modules/pdiff/README.txt$"
	"^modules/pdiff/RGBAImage.*$"
	"^modules/qslim/Mx.*$"
	"^po/POTFILES.in$"
	"^po/POTFILES.skip$"
	"^README$"
	"^share/fonts/.*[.]ttf$"
	"^share/ngui/rasterized/.*[.]png$"
	"^share/ngui/scalable/.*[.]svg$"
	"^share/scripts/.*$"
	"^.*[.]svn/.*$"
	"^tests/.*$"
	)

FILE(GLOB_RECURSE SOURCE_FILES *)
LIST(SORT SOURCE_FILES)

FOREACH(SOURCE_FILE ${SOURCE_FILES})
	FILE(RELATIVE_PATH RELATIVE_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${SOURCE_FILE})

	SET(TEST_FILE 1)
	FOREACH(SKIP_FILE ${SKIP_FILES})
		IF(RELATIVE_SOURCE_FILE MATCHES "${SKIP_FILE}")
			SET(TEST_FILE 0)
		ENDIF()
	ENDFOREACH()

	IF(TEST_FILE)
		STRING(TOLOWER "${RELATIVE_SOURCE_FILE}" LOWER_CASE_SOURCE_FILE)

		IF(NOT LOWER_CASE_SOURCE_FILE STREQUAL RELATIVE_SOURCE_FILE)
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE} must be all-lower-case")
		ENDIF()
	ENDIF()

ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} invalid filenames.")	
ENDIF()

