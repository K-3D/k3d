PROJECT(k3dsdk-qtui)

FILE(GLOB HEADERS *.h)
FILE(GLOB SOURCES *.cpp)

LIST(SORT HEADERS)
LIST(SORT SOURCES)

QT4_WRAP_CPP(MOC_BUILT_SOURCES
	action.h
	application.h
	application_widget.h
	console.h
	document_widget.h
	mode.h
	nag_message_dialog.h
	node_list_model.h
  property_widget_factory.h
	)

QT4_WRAP_UI(UI_BUILT_SOURCES
	nag_message_dialog.ui
	)

ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${QT_INCLUDES})

K3D_ADD_LIBRARY(k3dsdk-qtui SHARED ${HEADERS} ${SOURCES} ${MOC_BUILT_SOURCES} ${UI_BUILT_SOURCES})
K3D_GENERATE_DEF_FILE(k3dsdk-qtui)

TARGET_LINK_LIBRARIES(k3dsdk-qtui k3dsdk)
TARGET_LINK_LIBRARIES(k3dsdk-qtui ${QT_QTCORE_LIBRARY})
TARGET_LINK_LIBRARIES(k3dsdk-qtui ${QT_QTGUI_LIBRARY})
TARGET_LINK_LIBRARIES(k3dsdk-qtui ${QT_QTOPENGL_LIBRARY})
TARGET_LINK_LIBRARIES(k3dsdk-qtui ${QT_QTSCRIPT_LIBRARY})

INSTALL(TARGETS k3dsdk-qtui
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR}
	)

ADD_SUBDIRECTORY(script)

INCLUDE(ExternalProject)
FIND_PROGRAM(GIT_COMMAND git)

ExternalProject_Add(qtscriptgenerator
	DOWNLOAD_COMMAND git clone git://gitorious.org/qt-labs/qtscriptgenerator.git
	UPDATE_COMMAND ""
	CONFIGURE_COMMAND ${CMAKE_COMMAND} -E chdir <SOURCE_DIR>/generator ${QT_QMAKE_EXECUTABLE}
	BUILD_COMMAND ${CMAKE_COMMAND} -E chdir <SOURCE_DIR>/generator make -j4
	BUILD_IN_SOURCE 1
	INSTALL_COMMAND ""
	)

STRING(REPLACE ";" ":" GENERATOR_INCLUDE_PATHS "${QT_INCLUDES}")

ExternalProject_Add_Step(qtscriptgenerator generate-bindings
	DEPENDEES build
	WORKING_DIRECTORY "<SOURCE_DIR>/generator"
	COMMAND <SOURCE_DIR>/generator/generator --include-paths=${GENERATOR_INCLUDE_PATHS}
	)

ExternalProject_Add_Step(qtscriptgenerator configure-bindings
	DEPENDEES generate-bindings
	WORKING_DIRECTORY "<SOURCE_DIR>/qtbindings"
	COMMAND ${QT_QMAKE_EXECUTABLE} -after "SUBDIRS-=qtscript_sql qtscript_phonon qtscript_webkit"
	)

ExternalProject_Add_Step(qtscriptgenerator build-bindings
	DEPENDEES configure-bindings
	WORKING_DIRECTORY "<SOURCE_DIR>/qtbindings"
	COMMAND make -j4
	DEPENDERS install
	)

ExternalProject_Get_Property(qtscriptgenerator SOURCE_DIR)
SET(K3D_EXTRA_QT_PLUGINS "${SOURCE_DIR}/plugins" CACHE PATH "List of semicolon-delimited Qt plugin directories.")

