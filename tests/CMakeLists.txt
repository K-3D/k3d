PROJECT(k3d-tests)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/testing.py ${CMAKE_CURRENT_BINARY_DIR}/testing.py)

SET(K3D_NGUI_BINARY ${k3d_BINARY_DIR}/lib/k3d/uiplugins/k3d-ngui.module)
SET(K3D_NUI_BINARY ${k3d_BINARY_DIR}/lib/k3d/uiplugins/k3d-nui.module)

MACRO(K3D_ADD_TEST TEST_NAME)
	ADD_TEST(${TEST_NAME}
		${k3d_BINARY_DIR}/bin/k3d
		--log-level=debug
		--plugins=${k3d_BINARY_DIR}/lib/k3d/plugins
		--share=${k3d_SOURCE_DIR}/share
		--ui=${K3D_NUI_BINARY}
		${ARGN}
		)
ENDMACRO(K3D_ADD_TEST)

MACRO(K3D_ADD_SCRIPTED_TEST SCRIPT_NAME)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT_NAME})
	ADD_TEST(${SCRIPT_NAME}
		${k3d_BINARY_DIR}/bin/k3d
		--log-level=debug
		--plugins=${k3d_BINARY_DIR}/lib/k3d/plugins
		--share=${k3d_SOURCE_DIR}/share
		--ui=${K3D_NUI_BINARY}
		--script=${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT_NAME}
		${ARGN}
		)
ENDMACRO(K3D_ADD_SCRIPTED_TEST)

MACRO(K3D_ADD_PYTHON_TEST SCRIPT_NAME)
	IF(K3D_BUILD_PYTHON_MODULES)
		IF(K3D_TEST_VBO)
			CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT_NAME})
			ADD_TEST(${SCRIPT_NAME}
				${k3d_BINARY_DIR}/bin/k3d
				--log-level=debug
				--plugins=${k3d_BINARY_DIR}/lib/k3d/plugins
				--share=${k3d_SOURCE_DIR}/share
				--ui=${K3D_NUI_BINARY}
				--script=${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT_NAME}
				--setenv "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}"
				--setenv "USE_VBO=True"
				${ARGN}
				)
		ELSE(K3D_TEST_VBO)
			CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT_NAME})
			ADD_TEST(${SCRIPT_NAME}
				${k3d_BINARY_DIR}/bin/k3d
				--log-level=debug
				--plugins=${k3d_BINARY_DIR}/lib/k3d/plugins
				--share=${k3d_SOURCE_DIR}/share
				--ui=${K3D_NUI_BINARY}
				--script=${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT_NAME}
				--setenv "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}"
				${ARGN}
				)
		ENDIF(K3D_TEST_VBO)
	ENDIF(K3D_BUILD_PYTHON_MODULES)
ENDMACRO(K3D_ADD_PYTHON_TEST)

MACRO(K3D_ADD_SCRIPTED_NGUI_TEST SCRIPT_NAME)
	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT_NAME})
	ADD_TEST(${SCRIPT_NAME}
		${k3d_BINARY_DIR}/bin/k3d
		--log-level=debug
		--plugins=${k3d_BINARY_DIR}/lib/k3d/plugins
		--share=${k3d_SOURCE_DIR}/share
		--ui=${K3D_NGUI_BINARY}
		--batch
		--no-custom-layouts
		--no-splash
		--script=${CMAKE_CURRENT_BINARY_DIR}/${SCRIPT_NAME}
		${ARGN}
		--exit
		)
ENDMACRO(K3D_ADD_SCRIPTED_NGUI_TEST)

MACRO(K3D_ADD_NGUI_TUTORIAL_TEST TUTORIAL_NAME TUTORIAL_SCRIPT)
	ADD_TEST(${TUTORIAL_NAME}
		${k3d_BINARY_DIR}/bin/k3d
		--log-level=debug
		--plugins=${k3d_BINARY_DIR}/lib/k3d/plugins
		--share=${k3d_SOURCE_DIR}/share
		--ui=${K3D_NGUI_BINARY}
		--batch
		--no-custom-layouts
		--no-splash
		--script=${k3d_SOURCE_DIR}/share/tutorials/${TUTORIAL_SCRIPT}
		${ARGN}
		--exit
		)
ENDMACRO(K3D_ADD_NGUI_TUTORIAL_TEST)

K3D_ADD_TEST(test.startup --exit)

K3D_ADD_PYTHON_TEST(test.dart.measurements.py)

K3D_ADD_SCRIPTED_TEST(test.script.detect.lang.null)
SET_TESTS_PROPERTIES(test.script.detect.lang.null PROPERTIES WILL_FAIL TRUE)

K3D_ADD_SCRIPTED_TEST(test.script.detect.lang.k3dscript)
K3D_ADD_SCRIPTED_TEST(test.script.detect.lang.python.py)

K3D_ADD_SCRIPTED_TEST(test.script.failure.lang.k3dscript)
SET_TESTS_PROPERTIES(test.script.failure.lang.k3dscript PROPERTIES WILL_FAIL TRUE)

K3D_ADD_SCRIPTED_TEST(test.script.failure.lang.python.py)
SET_TESTS_PROPERTIES(test.script.failure.lang.python.py PROPERTIES WILL_FAIL TRUE)

K3D_ADD_SCRIPTED_TEST(test.script.failure.lang.python.exception.py)
SET_TESTS_PROPERTIES(test.script.failure.lang.python.exception.py PROPERTIES WILL_FAIL TRUE)

K3D_ADD_PYTHON_TEST(document.create.py)
K3D_ADD_PYTHON_TEST(document.stress.high_node_count.py)
K3D_ADD_PYTHON_TEST(document.stress.all_node_types.py)
K3D_ADD_PYTHON_TEST(document.stress.read_all_properties.py)

K3D_ADD_PYTHON_TEST(bitmap.importer.jpeg.py)
K3D_ADD_PYTHON_TEST(bitmap.importer.png.py)
K3D_ADD_PYTHON_TEST(bitmap.importer.tiff.py)

IF(K3D_BUILD_IMAGEMAGICK_MODULE)
	K3D_ADD_PYTHON_TEST(bitmap.importer.bmp.py)
	K3D_ADD_PYTHON_TEST(bitmap.importer.sun.py)
ENDIF(K3D_BUILD_IMAGEMAGICK_MODULE)

K3D_ADD_PYTHON_TEST(bitmap.reader.jpeg.py)
K3D_ADD_PYTHON_TEST(bitmap.reader.png.py)
K3D_ADD_PYTHON_TEST(bitmap.reader.tiff.py)

IF(K3D_BUILD_GTS_MODULE)
	K3D_ADD_PYTHON_TEST(mesh.source.GTSPolySphere.py)
ENDIF(K3D_BUILD_GTS_MODULE)

K3D_ADD_PYTHON_TEST(mesh.source.KnotCurve.py)
K3D_ADD_PYTHON_TEST(mesh.source.LSystemParser.py)
K3D_ADD_PYTHON_TEST(mesh.source.LissajousCurve.py)
K3D_ADD_PYTHON_TEST(mesh.source.Newell.teacup.py)
K3D_ADD_PYTHON_TEST(mesh.source.Newell.teapot.py)
K3D_ADD_PYTHON_TEST(mesh.source.Newell.teaspoon.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsCircle.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsCone.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsCylinder.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsDisk.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsGrid.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsHyperboloid.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsParaboloid.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsPolygon.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsSphere.py)
K3D_ADD_PYTHON_TEST(mesh.source.NurbsTorus.py)
K3D_ADD_PYTHON_TEST(mesh.source.PolyCone.py)
K3D_ADD_PYTHON_TEST(mesh.source.PolyCube.py)
K3D_ADD_PYTHON_TEST(mesh.source.PolyCushion.py)
K3D_ADD_PYTHON_TEST(mesh.source.PolyCylinder.py)
K3D_ADD_PYTHON_TEST(mesh.source.PolyDisk.py)
K3D_ADD_PYTHON_TEST(mesh.source.PolySphere.py)
K3D_ADD_PYTHON_TEST(mesh.source.PolyText.py)
K3D_ADD_PYTHON_TEST(mesh.source.PolyTorus.py)
K3D_ADD_PYTHON_TEST(mesh.source.Polyhedron.py)
K3D_ADD_PYTHON_TEST(mesh.source.SphereEversion.py)

K3D_ADD_PYTHON_TEST(mesh.diff.py)
K3D_ADD_PYTHON_TEST(mesh.diff.bevel_points.py)
K3D_ADD_PYTHON_TEST(mesh.diff.connect_vertices.py)
K3D_ADD_PYTHON_TEST(mesh.diff.dissolve.py)
K3D_ADD_PYTHON_TEST(mesh.diff.extrude_faces.py)
K3D_ADD_PYTHON_TEST(mesh.diff.subdivide_edges.py)

K3D_ADD_PYTHON_TEST(mesh.conversion.point_group.py)
K3D_ADD_PYTHON_TEST(mesh.conversion.curve.linear.py)
K3D_ADD_PYTHON_TEST(mesh.conversion.curve.cubic.py)
K3D_ADD_PYTHON_TEST(mesh.conversion.curve.nurbs.py)
K3D_ADD_PYTHON_TEST(mesh.conversion.patch.bicubic.py)
K3D_ADD_PYTHON_TEST(mesh.conversion.patch.bilinear.py)
K3D_ADD_PYTHON_TEST(mesh.conversion.patch.nurbs.py)
K3D_ADD_PYTHON_TEST(mesh.conversion.polyhedron.py)

K3D_ADD_PYTHON_TEST(mesh.writer.obj.py)
K3D_ADD_PYTHON_TEST(mesh.writer.k3d.py)

K3D_ADD_PYTHON_TEST(mesh.selection.null.py)
K3D_ADD_PYTHON_TEST(mesh.selection.none.py)
K3D_ADD_PYTHON_TEST(mesh.selection.all.py)
K3D_ADD_PYTHON_TEST(mesh.selection.points.py)

K3D_ADD_PYTHON_TEST(mesh.modifier.BendPoints.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.BulgePoints.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.CylindricalWavePoints.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.ExtrudeFaces.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.LinearPointNoise.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.LinearWavePoints.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.RotatePoints.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.ScalePoints.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.ShearPoints.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.SmoothPoints.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.TriangulateFaces.py)
K3D_ADD_PYTHON_TEST(mesh.modifier.TwistPoints.py)

IF(K3D_BUILD_GLX_MODULE)
	K3D_ADD_PYTHON_TEST(offscreen.glx.py)
ENDIF(K3D_BUILD_GLX_MODULE)

IF(K3D_BUILD_WGL_MODULE)
	K3D_ADD_PYTHON_TEST(offscreen.wgl.py)
ENDIF(K3D_BUILD_WGL_MODULE)

IF(K3D_BUILD_OFFSCREEN_MODULE)
	K3D_ADD_PYTHON_TEST(offscreen.py)
ENDIF(K3D_BUILD_OFFSCREEN_MODULE)

K3D_ADD_PYTHON_TEST(python.api.angle_axis.py)
K3D_ADD_PYTHON_TEST(python.api.get_dependency.py)
K3D_ADD_PYTHON_TEST(python.api.property.value.py)
K3D_ADD_PYTHON_TEST(python.api.node.selection.py)

IF(K3D_BUILD_NGUI_MODULE)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.startup)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.menu.activate.001)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.menu.activate.002)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.add_viewport)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.assign_hotkeys.py)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.bitmaps.add.py)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.bitmaps.color_monochrome.py)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.bitmaps.gamma.py)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.bitmaps.matte_colordiff.py)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.bitmaps.multiply.py)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.bitmaps.subtract.py)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.bitmaps.threshold.py)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.shaders.undo_selection.py)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.tools.unparent)
	K3D_ADD_SCRIPTED_NGUI_TEST(ngui.tools.undo_delete_rotate.py)

	K3D_ADD_NGUI_TUTORIAL_TEST(ngui.tutorials.getting_started getting_started.k3dscript)
	K3D_ADD_NGUI_TUTORIAL_TEST(ngui.tutorials.panel_basics panel_basics.k3dscript)
	K3D_ADD_NGUI_TUTORIAL_TEST(ngui.tutorials.navigation navigation.k3dscript)
	K3D_ADD_NGUI_TUTORIAL_TEST(ngui.tutorials.basic_selection basic_selection.k3dscript)
	K3D_ADD_NGUI_TUTORIAL_TEST(ngui.tutorials.gts_boolean gts_boolean.py)
ENDIF(K3D_BUILD_NGUI_MODULE)

