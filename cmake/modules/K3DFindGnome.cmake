FIND_PATH(K3D_BONOBO_ACTIVATION_INCLUDE_DIR bonobo-activation
	/usr/include/bonobo-activation-2.0
	DOC "Directory where bonobo-activation header files are located"
	)
MARK_AS_ADVANCED(K3D_BONOBO_ACTIVATION_INCLUDE_DIR)

FIND_PATH(K3D_BONOBO_INCLUDE_DIR libbonobo.h
	/usr/include/libbonobo-2.0
	DOC "Directory where the bonobo header files are located"
	)
MARK_AS_ADVANCED(K3D_BONOBO_INCLUDE_DIR)

FIND_PATH(K3D_GNOME_VFS_CONFIG_INCLUDE_DIR gnome-vfs-file-size.h
	/usr/${K3D_LIBDIR}/gnome-vfs-2.0/include
	DOC "Directory where the gnome vfs config file is located"
	)
MARK_AS_ADVANCED(K3D_GNOME_VFS_CONFIG_INCLUDE_DIR)

FIND_PATH(K3D_GNOME_VFS_INCLUDE_DIR libgnomevfs
	/usr/include/gnome-vfs-2.0
	DOC "Directory where the gnome vfs header files are located"
	)
MARK_AS_ADVANCED(K3D_GNOME_VFS_INCLUDE_DIR)

SET(K3D_GNOME_VFS_LIB gnomevfs-2 CACHE STRING "")
MARK_AS_ADVANCED(K3D_GNOME_VFS_LIB)

FIND_PATH(K3D_ORBIT_INCLUDE_DIR orbit
	/usr/include/orbit-2.0
	DOC "Directory where the orbit header files are located"
	)
MARK_AS_ADVANCED(K3D_ORBIT_INCLUDE_DIR)

SET(K3D_GNOME_VFS_INCLUDE_DIRS
	${K3D_BONOBO_ACTIVATION_INCLUDE_DIR}
	${K3D_BONOBO_INCLUDE_DIR}
	${K3D_GLIB_CONFIG_INCLUDE_DIR}
	${K3D_GLIB_INCLUDE_DIR}
	${K3D_GNOME_VFS_CONFIG_INCLUDE_DIR}
	${K3D_GNOME_VFS_INCLUDE_DIR}
	${K3D_ORBIT_INCLUDE_DIR}
	)

SET(K3D_GNOME_VFS_LIBS
	${K3D_GNOME_VFS_LIB}
	)

SET(K3D_GNOME_FOUND 1)

