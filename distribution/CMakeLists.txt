# Setup some odds-and-ends used for distribution
SET(K3D_SVN_ROOT "https://k3d.svn.sourceforge.net:/svnroot/k3d")
SET(K3D_SVN_TRUNK "${K3D_SVN_ROOT}/trunk")
SET(K3D_SVN_TAGS "${K3D_SVN_ROOT}/tags")
SET(K3D_SVN_BRANCHES "${K3D_SVN_ROOT}/branches")
SET(K3D_BRANCH_TAG "k3d-release-${MAJOR_VERSION}.${MINOR_VERSION}")
SET(K3D_VERSION_TAG "k3d-${MAJOR_VERSION}.${MINOR_VERSION}.${RELEASE_VERSION}.${BUILD_VERSION}")

ADD_CUSTOM_TARGET(changelog
	COMMAND ${SVNCOMMAND} log > ${k3d_BINARY_DIR}/ChangeLog
	WORKING_DIRECTORY ${k3d_SOURCE_DIR}
	)

ADD_CUSTOM_TARGET(tag-trunk
	COMMAND ${SVNCOMMAND} cp ${K3D_SVN_TRUNK} ${K3D_SVN_TAGS}/${K3D_VERSION_TAG} -m "COMP: Tagged K-3D Version ${VERSION}"
	WORKING_DIRECTORY ${k3d_SOURCE_DIR}
	)

ADD_CUSTOM_TARGET(tag-branch
	COMMAND ${SVNCOMMAND} cp ${K3D_SVN_TRUNK} ${K3D_SVN_BRANCHES}/${K3D_BRANCH_TAG} -m "COMP: Branched K-3D Version ${VERSION}"
	WORKING_DIRECTORY ${k3d_SOURCE_DIR}
	)

# The Win32 install includes its own copies of GTK and Boost, to prevent DLL hell ...
IF(WIN32)
	INSTALL(FILES ${K3D_BOOST_LIB_DIR}/boost_regex-mgw-1_33_1.dll DESTINATION bin)
	INSTALL(FILES ${K3D_BOOST_LIB_DIR}/boost_python-mgw-1_33_1.dll DESTINATION bin)

	INSTALL(DIRECTORY ${K3D_GTK_DIR}/bin/ DESTINATION bin
		USE_SOURCE_PERMISSIONS
		PATTERN *.pdb EXCLUDE
		PATTERN *-config EXCLUDE
		PATTERN *glade* EXCLUDE
		PATTERN *xml++* EXCLUDE
		PATTERN atkmm-1.6d.dll EXCLUDE
		PATTERN gdkmm-2.4d.dll EXCLUDE
		PATTERN gettext.sh EXCLUDE
		PATTERN glib-gettextize EXCLUDE
		PATTERN glib-mkenums EXCLUDE
		PATTERN glibmm-2.4d.dll EXCLUDE
		PATTERN gtkaio.sh EXCLUDE
		PATTERN gtkautogen EXCLUDE
		PATTERN gtkconfigure EXCLUDE
		PATTERN gtkmm-2.4d.dll EXCLUDE
		PATTERN msg*.exe EXCLUDE
		PATTERN pangomm-1.4d.dll EXCLUDE
		PATTERN sigc-2.0d.dll EXCLUDE
		)

	INSTALL(DIRECTORY ${K3D_GTK_DIR}/etc/ DESTINATION etc
		USE_SOURCE_PERMISSIONS
		)

	INSTALL(DIRECTORY ${K3D_GTK_DIR}/lib/ DESTINATION lib
		USE_SOURCE_PERMISSIONS
		PATTERN *.a EXCLUDE
		PATTERN *.lib EXCLUDE
		PATTERN *.pdb EXCLUDE
		PATTERN *glade* EXCLUDE
		PATTERN gdkmm-2.4 EXCLUDE
		PATTERN glib-2.0 EXCLUDE
		PATTERN glibmm-2.4 EXCLUDE
		PATTERN gtkglext-1.0 EXCLUDE
		PATTERN gtkmm-2.4 EXCLUDE
		PATTERN include EXCLUDE
		PATTERN libxml++-2.6 EXCLUDE
		PATTERN pkgconfig EXCLUDE
		PATTERN sigc++-2.0 EXCLUDE
		)

	# Override the default gtkrc file with our own
	INSTALL(FILES win32/gtkrc DESTINATION etc/gtk-2.0)

ENDIF(WIN32)

# Setup packaging ...
SET(CPACK_GENERATOR "STGZ")
SET(CPACK_PACKAGE_DESCRIPTION "K-3D free-as-in-freedom modeling, animation and rendering system")
SET(CPACK_PACKAGE_EXECUTABLES "k3d;K-3D ${VERSION}")
SET(CPACK_PACKAGE_FILE_NAME "k3d-setup-${VERSION}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "K-3D ${VERSION}")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "K-3D ${VERSION}")
SET(CPACK_PACKAGE_NAME "k3d")
SET(CPACK_PACKAGE_VENDOR "www.k-3d.org")
SET(CPACK_PACKAGE_VERSION ${VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
SET(CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
SET(CPACK_PACKAGE_VERSION_PATCH ${RELEASE_VERSION})
SET(CPACK_RESOURCE_FILE_LICENSE "${k3d_SOURCE_DIR}/COPYING")
SET(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "k3d-source-${VERSION}")

IF(WIN32)
	STRING(REPLACE "/" "\\\\" NATIVE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

	SET(CPACK_GENERATOR "NSIS")
	SET(CPACK_PACKAGE_ICON "${NATIVE_CURRENT_SOURCE_DIR}\\\\win32\\\\nsis\\\\headerimage.bmp")
	SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\k3d.exe")
	SET(CPACK_NSIS_HELP_LINK "http://www.k-3d.org/wiki/Support")

	SET(CPACK_SOURCE_STRIP_FILES
		bin/k3d.exe
		bin/k3d-bug-buddy.exe
		bin/k3d-make-module-proxy.exe
		bin/k3d-renderframe.exe
		bin/k3d-renderjob.exe
		bin/k3d-sl2xml.exe
		bin/k3d-uuidgen.exe
		bin/libk3dsdk-glew.dll
		bin/libk3dsdk-half.dll
		bin/libk3dsdk-ngui.dll
		bin/libk3dsdk-python-types.dll
		bin/libk3dsdk-python.dll
		bin/libk3dsdk-sgi-tesselator.dll
		bin/libk3dsdk-subdivision-surface.dll
		bin/libk3dsdk-surface-polygonizer.dll
		bin/libk3dsdk.dll
		lib/k3d/plugins/k3d-animation.module
		lib/k3d/plugins/k3d-annotation.module
		lib/k3d/plugins/k3d-aqsis.module
		lib/k3d/plugins/k3d-array.module
		lib/k3d/plugins/k3d-bitmap.module
		lib/k3d/plugins/k3d-blobbies.module
		lib/k3d/plugins/k3d-core.module
		lib/k3d/plugins/k3d-deformation.module
		lib/k3d/plugins/k3d-development.module
		lib/k3d/plugins/k3d-freetype2.module
		lib/k3d/plugins/k3d-graphviz.module
		lib/k3d/plugins/k3d-gts-io.module
		lib/k3d/plugins/k3d-jpeg-io.module
		lib/k3d/plugins/k3d-k3d-io.module
		lib/k3d/plugins/k3d-k3dscript.module
		lib/k3d/plugins/k3d-lsystem.module
		lib/k3d/plugins/k3d-mesh-instance.module
		lib/k3d/plugins/k3d-mesh.module
		lib/k3d/plugins/k3d-ngui-node-history-panel.module
		lib/k3d/plugins/k3d-ngui-node-list-panel.module
		lib/k3d/plugins/k3d-ngui-pipeline-panel.module
		lib/k3d/plugins/k3d-ngui-pipeline-profiler-panel.module
		lib/k3d/plugins/k3d-ngui-timeline-panel.module
		lib/k3d/plugins/k3d-ngui-undo-tree-panel.module
		lib/k3d/plugins/k3d-nurbs.module
		lib/k3d/plugins/k3d-obj-io.module
		lib/k3d/plugins/k3d-offscreen.module
		lib/k3d/plugins/k3d-opengl.module
		lib/k3d/plugins/k3d-pdiff.module
		lib/k3d/plugins/k3d-pixie.module
		lib/k3d/plugins/k3d-ply-io.module
		lib/k3d/plugins/k3d-png-io.module
		lib/k3d/plugins/k3d-polyhedra.module
		lib/k3d/plugins/k3d-primitives.module
		lib/k3d/plugins/k3d-python.module
		lib/k3d/plugins/k3d-qslim.module
		lib/k3d/plugins/k3d-quadrics.module
		lib/k3d/plugins/k3d-renderman-painters.module
		lib/k3d/plugins/k3d-renderman.module
		lib/k3d/plugins/k3d-scripting.module
		lib/k3d/plugins/k3d-selection.module
		lib/k3d/plugins/k3d-test.module
		lib/k3d/plugins/k3d-texture.module
		lib/k3d/plugins/k3d-tiff-io.module
		lib/k3d/plugins/k3d-time.module
		lib/k3d/plugins/k3d-virtual-gl-painters.module
		lib/k3d/plugins/k3d-wgl.module
		lib/k3d/plugins/k3d-windows.module
		lib/k3d/plugins/k3d-yafray.module
		lib/k3d/plugins/sample-plugin.module
		lib/k3d/uiplugins/k3d-ngui.module
		lib/k3d/uiplugins/k3d-nui.module
		lib/k3d/uiplugins/k3d-pyui.module
		)
	SET(CPACK_STRIP_FILES ${CPACK_SOURCE_STRIP_FILES})
ENDIF(WIN32)

INCLUDE(CPack)

# Setup the "all-in-one" package ...
IF(WIN32)
	OPTION(K3D_BUILD_ALL_IN_ONE_PACKAGE "Build the K-3D all-in-one package" OFF)

	IF(K3D_BUILD_ALL_IN_ONE_PACKAGE)
		SET(K3D_K3D_INSTALLER "k3d-setup-${VERSION}.exe")
		SET(K3D_K3D_INSTALLER_DIR ${k3d_BINARY_DIR})

		SET(K3D_AQSIS_INSTALLER "aqsis-setup-1.2.0.exe")
		FIND_PATH(K3D_AQSIS_INSTALLER_DIR ${K3D_AQSIS_INSTALLER} DOC "Directory containing ${K3D_AQSIS_INSTALLER}")
		MARK_AS_ADVANCED(K3D_AQSIS_INSTALLER_DIR)

		SET(K3D_PYTHON_INSTALLER "python-2.4.4.msi")
		FIND_PATH(K3D_PYTHON_INSTALLER_DIR ${K3D_PYTHON_INSTALLER} DOC "Directory containing ${K3D_PYTHON_INSTALLER}")
		MARK_AS_ADVANCED(K3D_PYTHON_INSTALLER_DIR)

		SET(K3D_PYGTK_INSTALLER "pygtk-2.10.4-1.win32-py2.4.exe")
		FIND_PATH(K3D_PYGTK_INSTALLER_DIR ${K3D_PYGTK_INSTALLER} DOC "Directory containing ${K3D_PYGTK_INSTALLER}") 
		MARK_AS_ADVANCED(K3D_PYGTK_INSTALLER_DIR)

#		SET(K3D_PYCAIRO_INSTALLER "pycairo-1.0.2-1.win32-py2.4.exe")
#		FIND_PATH(K3D_PYCAIRO_INSTALLER_DIR ${K3D_PYCAIRO_INSTALLER} DOC "Directory containing ${K3D_PYCAIRO_INSTALLER}")
#		MARK_AS_ADVANCED(K3D_PYCAIRO_INSTALLER_DIR)

		SET(K3D_CGKIT_INSTALLER "cgkit-2.0.0alpha7.win32-py2.4.exe")
		FIND_PATH(K3D_CGKIT_INSTALLER_DIR ${K3D_CGKIT_INSTALLER} DOC "Directory containing ${K3D_CGKIT_INSTALLER}")
		MARK_AS_ADVANCED(K3D_CGKIT_INSTALLER_DIR)

		SET(K3D_AQSIS_INSTALLER_PATH "${K3D_AQSIS_INSTALLER_DIR}/${K3D_AQSIS_INSTALLER}")
		SET(K3D_PYTHON_INSTALLER_PATH "${K3D_PYTHON_INSTALLER_DIR}/${K3D_PYTHON_INSTALLER}")
		SET(K3D_PYGTK_INSTALLER_PATH "${K3D_PYGTK_INSTALLER_DIR}/${K3D_PYGTK_INSTALLER}")
#		SET(K3D_PYCAIRO_INSTALLER_PATH "${K3D_PYCAIRO_INSTALLER_DIR}/${K3D_PYCAIRO_INSTALLER}")
		SET(K3D_CGKIT_INSTALLER_PATH "${K3D_CGKIT_INSTALLER_DIR}/${K3D_CGKIT_INSTALLER}")

		IF(NOT EXISTS "${K3D_AQSIS_INSTALLER_PATH}")
			MESSAGE(SEND_ERROR "Couldn't locate ${K3D_AQSIS_INSTALLER_PATH}")
		ENDIF(NOT EXISTS "${K3D_AQSIS_INSTALLER_PATH}")

		IF(NOT EXISTS "${K3D_PYTHON_INSTALLER_PATH}")
			MESSAGE(SEND_ERROR "Couldn't locate ${K3D_PYTHON_INSTALLER_PATH}")
		ENDIF(NOT EXISTS "${K3D_PYTHON_INSTALLER_PATH}")

		IF(NOT EXISTS "${K3D_PYGTK_INSTALLER_PATH}")
			MESSAGE(SEND_ERROR "Couldn't locate ${K3D_PYGTK_INSTALLER_PATH}")
		ENDIF(NOT EXISTS "${K3D_PYGTK_INSTALLER_PATH}")

#		IF(NOT EXISTS "${K3D_PYCAIRO_INSTALLER_PATH}")
#			MESSAGE(SEND_ERROR "Couldn't locate ${K3D_PYCAIRO_INSTALLER_PATH}")
#		ENDIF(NOT EXISTS "${K3D_PYCAIRO_INSTALLER_PATH}")

		IF(NOT EXISTS "${K3D_CGKIT_INSTALLER_PATH}")
			MESSAGE(SEND_ERROR "Couldn't locate ${K3D_CGKIT_INSTALLER_PATH}")
		ENDIF(NOT EXISTS "${K3D_CGKIT_INSTALLER_PATH}")

		CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/win32/nsis/all-in-one.nsi.in ${CMAKE_CURRENT_BINARY_DIR}/win32/nsis/all-in-one.nsi @ONLY)

		ADD_CUSTOM_TARGET(package-all-in-one
			COMMAND makensis ${CMAKE_CURRENT_BINARY_DIR}/win32/nsis/all-in-one.nsi
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			)

	ENDIF(K3D_BUILD_ALL_IN_ONE_PACKAGE)
ENDIF(WIN32)

