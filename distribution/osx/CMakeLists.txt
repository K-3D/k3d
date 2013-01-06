OPTION(K3D_CREATE_PACKAGEMAKER_PACKAGE "Include an option to build a MacOSX PackageMaker package." OFF)

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
    ${K3D_MACPORTS_DIR}/etc/gtk-2.0
    ${K3D_MACPORTS_DIR}/etc/fonts
    ${K3D_MACPORTS_DIR}/etc/pango
  DESTINATION
    etc
  )

# Dependencies from MacPorts not caught elsewhere
INSTALL(
  DIRECTORY
    ${K3D_MACPORTS_DIR}/lib/gdk-pixbuf-2.0
  DESTINATION
    lib
  USE_SOURCE_PERMISSIONS
  )

# Enable automatic installation of library dependencies ...
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/dependencies.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/dependencies.cmake" @ONLY)
INSTALL(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/dependencies.cmake")

# More accurate guess of the OS architecture
EXEC_PROGRAM(uname ARGS -m OUTPUT_VARIABLE K3D_OSX_ARCH)

# Bundle packaging is always available ...
ADD_SUBDIRECTORY(bundle)

# We optionally support creating packagemaker packages ...
IF(K3D_CREATE_PACKAGEMAKER_PACKAGE)
	ADD_SUBDIRECTORY(packagemaker)
ENDIF(K3D_CREATE_PACKAGEMAKER_PACKAGE)

