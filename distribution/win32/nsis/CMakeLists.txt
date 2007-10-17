STRING(REPLACE "/" "\\\\" NATIVE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

SET(CPACK_GENERATOR "NSIS")
SET(CPACK_PACKAGE_ICON "${NATIVE_CURRENT_SOURCE_DIR}\\\\headerimage.bmp")
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

INCLUDE(CPack)

# Setup the optional "all-in-one" package for Win32 ...
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

	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/all-in-one.nsi.in ${CMAKE_CURRENT_BINARY_DIR}/all-in-one.nsi @ONLY)

	ADD_CUSTOM_TARGET(package-all-in-one
		COMMAND makensis ${CMAKE_CURRENT_BINARY_DIR}/all-in-one.nsi
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)

ENDIF(K3D_BUILD_ALL_IN_ONE_PACKAGE)

