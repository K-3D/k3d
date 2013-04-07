PROJECT(k3dsdk-python)

INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR})
INCLUDE_DIRECTORIES(${k3dsdk_BINARY_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_GLIBMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_SIGC_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_PYTHON_INCLUDE_DIRS})

LINK_DIRECTORIES(${K3D_SIGC_LIB_DIRS})

K3D_ADD_LIBRARY(k3dsdk-python-arrays SHARED
	typed_array_python.cpp
	typed_array_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-arrays)
TARGET_LINK_LIBRARIES(k3dsdk-python-arrays
	k3dsdk
	${K3D_PYTHON_LIBS}
	${Boost_PYTHON_LIBRARY}
	)

K3D_ADD_LIBRARY(k3dsdk-python-const-arrays SHARED
	const_typed_array_python.cpp
	const_typed_array_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-const-arrays)
TARGET_LINK_LIBRARIES(k3dsdk-python-const-arrays
	k3dsdk
	${K3D_PYTHON_LIBS}
	${Boost_PYTHON_LIBRARY}
	)

K3D_ADD_LIBRARY(k3dsdk-python-values SHARED
	angle_axis_python.cpp
	angle_axis_python.h
	bitmap_python.cpp
	bitmap_python.h
	bounding_box3_python.cpp
	bounding_box3_python.h
	color_python.cpp
	color_python.h
	const_bitmap_python.cpp
	const_bitmap_python.h
	euler_angles_python.cpp
	euler_angles_python.h
	filesystem_python.cpp
	filesystem_python.h
	matrix4_python.cpp
	matrix4_python.h
	normal3_python.cpp
	normal3_python.h
	point2_python.cpp
	point2_python.h
	point3_python.cpp
	point3_python.h
	point4_python.cpp
	point4_python.h
	texture3_python.cpp
	texture3_python.h
	uuid_python.cpp
	uuid_python.h
	vector2_python.cpp
	vector2_python.h
	vector3_python.cpp
	vector3_python.h
	vector4_python.cpp
	vector4_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-values)
TARGET_LINK_LIBRARIES(k3dsdk-python-values
	k3dsdk
	${Boost_PYTHON_LIBRARY}
	${K3D_PYTHON_LIBS}
	)

K3D_ADD_LIBRARY(k3dsdk-python-mesh SHARED
	mesh_python.cpp
	mesh_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-mesh)
TARGET_LINK_LIBRARIES(k3dsdk-python-mesh k3dsdk-python-arrays)
TARGET_LINK_LIBRARIES(k3dsdk-python-mesh k3dsdk-python-const-arrays)

K3D_ADD_LIBRARY(k3dsdk-python-interfaces SHARED
	any_python.cpp
	any_python.h
	offscreen_context_factory_gl_python.cpp
	offscreen_context_factory_gl_python.h
	idocument_exporter_python.cpp
	idocument_exporter_python.h
	idocument_importer_python.cpp
	idocument_importer_python.h
	idocument_python.cpp
	idocument_python.h
	ifile_change_notifier_python.cpp
	ifile_change_notifier_python.h
	ikeyframer_python.cpp
	ikeyframer_python.h
	imesh_storage_python.cpp
	imesh_storage_python.h
	imeta_object_python.cpp
	imeta_object_python.h
	imetadata_python.cpp
	imetadata_python.h
	inode_python.cpp
	inode_python.h
	inode_selection_python.cpp
	inode_selection_python.h
	iplugin_factory_python.cpp
	iplugin_factory_python.h
	iproperty_collection_python.cpp
	iproperty_collection_python.h
	iproperty_python.cpp
	iproperty_python.h
	irender_camera_frame_python.cpp
	irender_camera_frame_python.h
	irender_camera_preview_python.cpp
	irender_camera_preview_python.h
	isnappable_python.cpp
	isnappable_python.h
	iunknown_python.cpp
	iunknown_python.h
	iuser_interface_python.cpp
	iuser_interface_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-interfaces)
TARGET_LINK_LIBRARIES(k3dsdk-python-interfaces k3dsdk-python-mesh)
TARGET_LINK_LIBRARIES(k3dsdk-python-interfaces k3dsdk-python-values)

K3D_ADD_LIBRARY(k3dsdk-python-b SHARED
	table_python.cpp
	table_python.h
	const_table_python.cpp
	const_table_python.h
	const_named_arrays_python.cpp
	const_named_arrays_python.h
	const_named_tables_python.cpp
	const_named_tables_python.h
	euler_python.cpp
	euler_python.h
	geometry_python.cpp
	geometry_python.h
	log_python.cpp
	log_python.h
	mime_python.cpp
	mime_python.h
	named_arrays_python.cpp
	named_arrays_python.h
	named_tables_python.cpp
	named_tables_python.h
	node_python.cpp
	node_python.h
	parallel_python.cpp
	plugin_python.cpp
	plugin_python.h
	property_python.cpp
	property_python.h
	resource_python.cpp
	resource_python.h
	ri_python.cpp
	ri_python.h
	selection_python.cpp
	selection_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-b)
TARGET_LINK_LIBRARIES(k3dsdk-python-b k3dsdk-python-arrays)
TARGET_LINK_LIBRARIES(k3dsdk-python-b k3dsdk-python-const-arrays)
TARGET_LINK_LIBRARIES(k3dsdk-python-b k3dsdk-python-interfaces)
TARGET_LINK_LIBRARIES(k3dsdk-python-b k3dsdk-resource)

K3D_ADD_LIBRARY(k3dsdk-python-primitives SHARED
	bezier_triangle_patch_python.cpp
	bezier_triangle_patch_python.h
	bicubic_patch_python.cpp
	bicubic_patch_python.h
	bilinear_patch_python.cpp
	bilinear_patch_python.h
	blobby_python.cpp
	blobby_python.h
	cone_python.cpp
	cone_python.h
	cubic_curve_python.cpp
	cubic_curve_python.h
	cylinder_python.cpp
	cylinder_python.h
	disk_python.cpp
	disk_python.h
	hyperboloid_python.cpp
	hyperboloid_python.h
	linear_curve_python.cpp
	linear_curve_python.h
	nurbs_curve_python.cpp
	nurbs_curve_python.h
	nurbs_patch_python.cpp
	nurbs_patch_python.h
	paraboloid_python.cpp
	paraboloid_python.h
	particle_python.cpp
	particle_python.h
	polyhedron_python.cpp
	polyhedron_python.h
	sphere_python.cpp
	sphere_python.h
	teapot_python.cpp
	teapot_python.h
	torus_python.cpp
	torus_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-primitives)
TARGET_LINK_LIBRARIES(k3dsdk-python-primitives k3dsdk-python-b)

K3D_ADD_LIBRARY(k3dsdk-python SHARED
	difference_python.cpp
	difference_python.h
	instance_wrapper_python.h
	file_signal_python.cpp
	file_signal_python.h
	object_model_python.cpp
	object_model_python.h
	script_python.cpp
	script_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python)
TARGET_LINK_LIBRARIES(k3dsdk-python
	k3dsdk-python-primitives
	${Boost_PYTHON_LIBRARY}
	)

INSTALL(TARGETS k3dsdk-python-arrays
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-const-arrays
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-values
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-mesh
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-interfaces
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-b
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-primitives
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

IF(K3D_BUILD_NGUI_MODULE)
	INCLUDE_DIRECTORIES(${K3D_GTKMM_INCLUDE_DIRS})

	K3D_ADD_LIBRARY(k3dsdk-python-atk SHARED
		atk_python.cpp
		atk_python.h
		)
	K3D_GENERATE_DEF_FILE(k3dsdk-python-atk)
	TARGET_LINK_LIBRARIES(k3dsdk-python-atk
		k3dsdk
		${Boost_PYTHON_LIBRARY}
		${K3D_PYTHON_LIBS}
		${K3D_GTKMM_LIBS}
		)

	TARGET_LINK_LIBRARIES(k3dsdk-python k3dsdk-python-atk)

	INSTALL(TARGETS k3dsdk-python-atk
		RUNTIME DESTINATION bin 
		LIBRARY DESTINATION ${K3D_LIBDIR}
		ARCHIVE DESTINATION ${K3D_LIBDIR})
ENDIF()

