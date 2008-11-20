PROJECT(k3dsdk-python)

INCLUDE_DIRECTORIES(${k3d_SOURCE_DIR})
INCLUDE_DIRECTORIES(${k3dsdk_SOURCE_DIR}/gil)
INCLUDE_DIRECTORIES(${k3dsdk_BINARY_DIR})
INCLUDE_DIRECTORIES(${K3D_BOOST_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_GLIBMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${K3D_SIGC_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

LINK_DIRECTORIES(${K3D_BOOST_LIB_DIRS})
LINK_DIRECTORIES(${K3D_SIGC_LIB_DIRS})

ADD_LIBRARY(k3dsdk-python-arrays SHARED
	typed_array_python.cpp
	typed_array_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-arrays)
TARGET_LINK_LIBRARIES(k3dsdk-python-arrays k3dsdk)
TARGET_LINK_LIBRARIES(k3dsdk-python-arrays ${K3D_BOOST_PYTHON_LIBS})
TARGET_LINK_LIBRARIES(k3dsdk-python-arrays ${PYTHON_LIBRARIES})

ADD_LIBRARY(k3dsdk-python-const-arrays SHARED
	const_typed_array_python.cpp
	const_typed_array_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-const-arrays)
TARGET_LINK_LIBRARIES(k3dsdk-python-const-arrays k3dsdk)
TARGET_LINK_LIBRARIES(k3dsdk-python-const-arrays ${K3D_BOOST_PYTHON_LIBS})
TARGET_LINK_LIBRARIES(k3dsdk-python-const-arrays ${PYTHON_LIBRARIES})

ADD_LIBRARY(k3dsdk-python-a SHARED
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
	log_python.cpp
	log_python.h
	matrix4_python.cpp
	matrix4_python.h
	mime_python.cpp
	mime_python.h
	normal3_python.cpp
	normal3_python.h
	path_python.cpp
	path_python.h
	point2_python.cpp
	point2_python.h
	point3_python.cpp
	point3_python.h
	point4_python.cpp
	point4_python.h
	ri_python.cpp
	ri_python.h
	texture3_python.cpp
	texture3_python.h
	uuid_python.cpp
	uuid_python.h
	vector2_python.cpp
	vector2_python.h
	vector3_python.cpp
	vector3_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-a)
TARGET_LINK_LIBRARIES(k3dsdk-python-a k3dsdk)
TARGET_LINK_LIBRARIES(k3dsdk-python-a ${K3D_BOOST_PYTHON_LIBS})
TARGET_LINK_LIBRARIES(k3dsdk-python-a ${PYTHON_LIBRARIES})

ADD_LIBRARY(k3dsdk-python-b SHARED
	attribute_arrays_python.cpp
	attribute_arrays_python.h
	const_attribute_arrays_python.cpp
	const_attribute_arrays_python.h
	const_named_arrays_python.cpp
	const_named_arrays_python.h
	const_named_attribute_arrays_python.cpp
	const_named_attribute_arrays_python.h
	icommand_node_python.cpp
	icommand_node_python.h
	imaterial_python.cpp
	imaterial_python.h
	imetadata_python.cpp
	imetadata_python.h
	inode_python.cpp
	inode_python.h
	instance_wrapper_python.h
	iplugin_factory_python.cpp
	iplugin_factory_python.h
	isnappable_python.cpp
	isnappable_python.h
	iunknown_python.cpp
	iunknown_python.h
	iuser_interface_python.cpp
	iuser_interface_python.h
	named_arrays_python.cpp
	named_arrays_python.h
	named_attribute_arrays_python.cpp
	named_attribute_arrays_python.h
	parallel_python.cpp
	plugin_python.cpp
	plugin_python.h
	resource_python.cpp
	resource_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-b)
TARGET_LINK_LIBRARIES(k3dsdk-python-b k3dsdk-python-arrays)
TARGET_LINK_LIBRARIES(k3dsdk-python-b k3dsdk-python-const-arrays)
TARGET_LINK_LIBRARIES(k3dsdk-python-b k3dsdk-resource)

ADD_LIBRARY(k3dsdk-python-d SHARED
	any_python.cpp
	any_python.h
	dynamic_cast_python.cpp
	dynamic_cast_python.h
	euler_python.cpp
	euler_python.h
	idocument_python.cpp
	idocument_python.h
	imesh_storage_python.cpp
	imesh_storage_python.h
	inode_selection_python.cpp
	inode_selection_python.h
	iproperty_collection_python.cpp
	iproperty_collection_python.h
	iproperty_python.cpp
	iproperty_python.h
	mesh_python.cpp
	mesh_python.h
	mesh_selection_python.cpp
	mesh_selection_python.h
	node_python.cpp
	node_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python-d)
TARGET_LINK_LIBRARIES(k3dsdk-python-d k3dsdk)
TARGET_LINK_LIBRARIES(k3dsdk-python-d ${K3D_BOOST_PYTHON_LIBS})
TARGET_LINK_LIBRARIES(k3dsdk-python-d ${PYTHON_LIBRARIES})
TARGET_LINK_LIBRARIES(k3dsdk-python-d k3dsdk-python-a)
TARGET_LINK_LIBRARIES(k3dsdk-python-d k3dsdk-python-b)

ADD_LIBRARY(k3dsdk-python-primitives SHARED
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
	point_group_python.cpp
	point_group_python.h
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
TARGET_LINK_LIBRARIES(k3dsdk-python-primitives k3dsdk-python-d)

ADD_LIBRARY(k3dsdk-python SHARED
	object_model_python.cpp
	object_model_python.h
	)
K3D_GENERATE_DEF_FILE(k3dsdk-python)
TARGET_LINK_LIBRARIES(k3dsdk-python k3dsdk-python-primitives)

INSTALL(TARGETS k3dsdk-python-arrays
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-const-arrays
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-a
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-b
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python-d
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

INSTALL(TARGETS k3dsdk-python
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION ${K3D_LIBDIR}
	ARCHIVE DESTINATION ${K3D_LIBDIR})

