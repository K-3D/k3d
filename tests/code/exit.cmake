# Test to ensure that code doesn't call exit()

SET(ERROR_COUNT 0)
SET(SKIP_FILES
	"^[.]hg/.*$"
	"[.]yaml$"
	"^application/k3d_main.cpp$"
	"^k3dsdk/application_detail.cpp$"
	"^k3dsdk/application_detail.h$"
	"^k3dsdk/ftgl/ChangeLog$"
	"^k3dsdk/ftgl/Makefile.in$"
	"^k3dsdk/iapplication.h$"
	"^k3dsdk/ngui/application_state.cpp$"
	"^k3dsdk/python/object_model_python.cpp$"
	"^man/k3d.1.in$"
	"^modules/inotify/inotify-cxx.h$"
	"^modules/ngui_python_shell/dialog.cpp$"
	"^modules/obj_io/obj_specification.txt$"
	"^modules/qslim/MxStdModel.cpp$"
	"^renderframe/main.cpp$"
	"^renderjob/main.cpp$"
	"^resource-compiler/main.cpp$"
	"^share/lsystem/fractint2.ls$"
	"^share/scripts/shell/batch_render.py$"
	"^share/scripts/shell/simple_rpc.py$"
	"^sl2xml/main.cpp$"
	"^tests/CMakeLists.txt$"
	"^tests/code/CMakeLists.txt$"
	"^tests/code/exit.cmake$"
	"^tests/document.stress.read_all_properties.py$"
	"^tests/python/module.py$"
	)

FILE(GLOB_RECURSE SOURCE_FILES "*")
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
		FILE(READ ${SOURCE_FILE} SOURCE)
		IF(SOURCE MATCHES "exit")
			MATH(EXPR ERROR_COUNT "${ERROR_COUNT} + 1")
			MESSAGE("${SOURCE_FILE}")
		ENDIF()
	ENDIF()
ENDFOREACH(SOURCE_FILE)

IF(ERROR_COUNT)
	MESSAGE(SEND_ERROR "Found ${ERROR_COUNT} files calling exit(), which should never happen.  Contact the mailing list for guidance.")	
ENDIF()

