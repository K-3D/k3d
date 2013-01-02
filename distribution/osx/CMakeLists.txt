#OPTION(K3D_INSTALL_AQSIS "Include Aqsis in the K-3D installer" OFF)
#OPTION(K3D_INSTALL_CGKIT "Include cgkit in the K-3D installer" OFF)
#OPTION(K3D_INSTALL_PYGTK "Include PyGTK in the K-3D installer" OFF)
#OPTION(K3D_INSTALL_PYOPENGL "Include PyOpenGL in the K-3D installer" OFF)
#OPTION(K3D_INSTALL_PYTHON "Include Python in the K-3D installer" ON)
OPTION(K3D_CREATE_PACKAGEMAKER_PACKAGE "Include an option to build a MacOSX PackageMaker package." OFF)

SET(K3D_BINARY_LOCATIONS )
LIST(APPEND K3D_BINARY_LOCATIONS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/k3d")
LIST(APPEND K3D_BINARY_LOCATIONS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/k3d-make-module-proxy")
LIST(APPEND K3D_BINARY_LOCATIONS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/k3d-renderframe")
LIST(APPEND K3D_BINARY_LOCATIONS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/k3d-renderjob")
LIST(APPEND K3D_BINARY_LOCATIONS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/k3d-resource-compiler")
LIST(APPEND K3D_BINARY_LOCATIONS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/k3d-sl2xml")
LIST(APPEND K3D_BINARY_LOCATIONS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/k3d-uuidgen")

FILE(GLOB_RECURSE K3D_PLUGIN_BINARIES "${k3d_BINARY_DIR}/${K3D_LIBDIR}/k3d/plugins/*.module")
LIST(APPEND K3D_BINARY_LOCATIONS ${K3D_PLUGIN_BINARIES})

# Enable automatic installation of library dependencies ...
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/dependencies.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/dependencies.cmake" @ONLY)
INSTALL(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/dependencies.cmake")

# Install some runtime dependencies that can't be detected via static analysis ...

# This is an ugly hack, we should be able to detect this automatically, but I'm feeling lazy ...
SET(K3D_MACPORTS_DIR "/opt/local" CACHE PATH "Root directory containing installed MacPorts packages")

INSTALL(
	PROGRAMS
		${K3D_MACPORTS_DIR}/lib/libgailutil.18.dylib
	DESTINATION
		lib
	)

INSTALL(
	DIRECTORY
		${K3D_MACPORTS_DIR}/lib/gtk-2.0/2.10.0
	DESTINATION
		lib/gtk-2.0
	USE_SOURCE_PERMISSIONS
	)

INSTALL(
	DIRECTORY
		${K3D_MACPORTS_DIR}/lib/gtk-2.0/modules
	DESTINATION
		lib/gtk-2.0/2.10.0
	USE_SOURCE_PERMISSIONS
	)

INSTALL(
	DIRECTORY
		${K3D_MACPORTS_DIR}/lib/pango
	DESTINATION
		lib
	USE_SOURCE_PERMISSIONS
	)

INSTALL(
  DIRECTORY
  	${K3D_MACPORTS_DIR}/lib/gdk-pixbuf-2.0
  DESTINATION
  	lib
  USE_SOURCE_PERMISSIONS
  )

INSTALL(
	DIRECTORY
		${K3D_MACPORTS_DIR}/etc/gtk-2.0
		${K3D_MACPORTS_DIR}/etc/fonts
		${K3D_MACPORTS_DIR}/etc/pango
	DESTINATION
		etc
	)

# Bundle packaging is always available ...
ADD_SUBDIRECTORY(bundle)

# We optionally support creating packagemaker packages ...
IF(K3D_CREATE_PACKAGEMAKER_PACKAGE)
	ADD_SUBDIRECTORY(packagemaker)
ENDIF(K3D_CREATE_PACKAGEMAKER_PACKAGE)

