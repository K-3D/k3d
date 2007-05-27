SET(K3D_GTKMM_FOUND 0)

######################################################################
# Posix specific configuration

IF(UNIX)
	INCLUDE(FindPkgConfig)
	PKG_CHECK_MODULES(GTKMM gtkmm-2.4)

	IF(GTKMM_FOUND)
		SET(K3D_GTKMM_INCLUDE_DIRS
			${GTKMM_INCLUDE_DIRS}
			)

		SET(K3D_GTKMM_LIB_DIRS
			${GTKMM_LIBRARY_DIRS}
			)

		SET(K3D_GTKMM_LIBS
			${GTKMM_LIBRARIES}
			)

		SET(K3D_GTKMM_FOUND 1)
	ENDIF(GTKMM_FOUND)

ENDIF(UNIX)

######################################################################
# Win32 specific configuration

IF(WIN32)
	SET(K3D_GTK_DIR "c:/gtk" CACHE PATH "Top-level directory where the gtk/gtkmm libraries are located")
	MARK_AS_ADVANCED(K3D_GTK_DIR)

	# Configure gdk ...
	SET(K3D_GDK_LIB gdk-win32-2.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GDK_LIB)

	# Configure gtk ...
	SET(K3D_GTK_LIB_DIR ${K3D_GTK_DIR}/lib CACHE PATH "Directory where the gtk/gtkmm libraries are located")
	MARK_AS_ADVANCED(K3D_GTK_LIB_DIR)

	SET(K3D_GTK_LIB gtk-win32-2.0 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GTK_LIB)

	# Configure atk ...
	FIND_PATH(K3D_ATK_INCLUDE_DIR atk
		${K3D_GTK_DIR}/include/atk-1.0
		DOC "Directory where the atk header files are located"
		)
	MARK_AS_ADVANCED(K3D_ATK_INCLUDE_DIR)

	# Configure atkmm ...
	FIND_PATH(K3D_ATKMM_INCLUDE_DIR atkmm.h
		${K3D_GTK_DIR}/include/atkmm-1.6
		DOC "Directory where the atkmm header files are located"
		)
	MARK_AS_ADVANCED(K3D_ATKMM_INCLUDE_DIR)

	SET(K3D_ATKMM_LIB atkmm-1.6 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_ATKMM_LIB)

	# Configure cairo ...
	FIND_PATH(K3D_CAIRO_INCLUDE_DIR cairo.h
		${K3D_GTK_DIR}/include/cairo
		DOC "Directory where the cairo header files are located"
		)
	MARK_AS_ADVANCED(K3D_CAIRO_INCLUDE_DIR)

	# Configure gdk ...
	FIND_PATH(K3D_GDK_INCLUDE_DIR gdk.h
		${K3D_GTK_DIR}/include/gtk-2.0/gdk
		DOC "Directory where the gdk header files are located"
		)
	MARK_AS_ADVANCED(K3D_GDK_INCLUDE_DIR)

	# Configure gdkmm ...
	FIND_PATH(K3D_GDKMM_CONFIG_INCLUDE_DIR gdkmmconfig.h
		${K3D_GTK_DIR}/lib/gdkmm-2.4/include
		DOC "Directory where the gdkmm config file is located"
		)
	MARK_AS_ADVANCED(K3D_GDKMM_CONFIG_INCLUDE_DIR)

	FIND_PATH(K3D_GDKMM_INCLUDE_DIR gdkmm.h
		${K3D_GTK_DIR}/include/gdkmm-2.4
		DOC "Directory where the gdkmm header files are located"
		)
	MARK_AS_ADVANCED(K3D_GDKMM_INCLUDE_DIR)

	SET(K3D_GDKMM_LIB gdkmm-2.4 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GDKMM_LIB)

	# Configure gtk ...
	FIND_PATH(K3D_GTK_CONFIG_INCLUDE_DIR gdkconfig.h
		${K3D_GTK_DIR}/lib/gtk-2.0/include
		DOC "Directory where the gtk config file is located"
		)
	MARK_AS_ADVANCED(K3D_GTK_CONFIG_INCLUDE_DIR)

	FIND_PATH(K3D_GTK_INCLUDE_DIR gtk
		${K3D_GTK_DIR}/include/gtk-2.0
		DOC "Directory where the gtk header files are located"
		)
	MARK_AS_ADVANCED(K3D_GTK_INCLUDE_DIR)

	# Configure gtkmm ...
	FIND_PATH(K3D_GTKMM_CONFIG_INCLUDE_DIR gtkmmconfig.h
		${K3D_GTK_DIR}/lib/gtkmm-2.4/include
		DOC "Directory where the gtkmm config file is located"
		)
	MARK_AS_ADVANCED(K3D_GTKMM_CONFIG_INCLUDE_DIR)

	FIND_PATH(K3D_GTKMM_INCLUDE_DIR gtkmm.h
		${K3D_GTK_DIR}/include/gtkmm-2.4
		DOC "Directory where the gtkmm header files are located"
		)
	MARK_AS_ADVANCED(K3D_GTKMM_INCLUDE_DIR)

	SET(K3D_GTKMM_LIB gtkmm-2.4 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_GTKMM_LIB)

	# Configure pango ...
	FIND_PATH(K3D_PANGO_INCLUDE_DIR pango
		${K3D_GTK_DIR}/include/pango-1.0
		DOC "Directory where the pango header files are located"
		)
	MARK_AS_ADVANCED(K3D_PANGO_INCLUDE_DIR)

	# Configure pangomm ...
	FIND_PATH(K3D_PANGOMM_INCLUDE_DIR pangomm.h
		${K3D_GTK_DIR}/include/pangomm-1.4
		DOC "Directory where the pangomm header files are located"
		)
	MARK_AS_ADVANCED(K3D_PANGOMM_INCLUDE_DIR)

	SET(K3D_PANGOMM_LIB pangomm-1.4 CACHE STRING "")
	MARK_AS_ADVANCED(K3D_PANGOMM_LIB)

	SET(K3D_GTKMM_INCLUDE_DIRS
		${K3D_ATKMM_INCLUDE_DIR}
		${K3D_ATK_INCLUDE_DIR}
		${K3D_CAIRO_INCLUDE_DIR}
		${K3D_GDKMM_CONFIG_INCLUDE_DIR}
		${K3D_GDKMM_INCLUDE_DIR}
		${K3D_GDK_INCLUDE_DIR}
		${K3D_GTKMM_CONFIG_INCLUDE_DIR}
		${K3D_GTKMM_INCLUDE_DIR}
		${K3D_GTK_CONFIG_INCLUDE_DIR}
		${K3D_GTK_INCLUDE_DIR}
		${K3D_PANGOMM_INCLUDE_DIR}
		${K3D_PANGO_INCLUDE_DIR}
		)

	SET(K3D_GTKMM_LIB_DIRS
		${K3D_GTK_LIB_DIR}
		)

	SET(K3D_GTKMM_LIBS
		${K3D_ATKMM_LIB}
		${K3D_GDKMM_LIB}
		${K3D_GDK_LIB}
		${K3D_GTKMM_LIB}
		${K3D_GTK_LIB}
		${K3D_PANGOMM_LIB}
		)

	SET(K3D_GTKMM_FOUND 1)

ENDIF(WIN32)

