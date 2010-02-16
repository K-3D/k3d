K3D_TEST(startup.k3d-uuidgen
	TARGET k3d-uuidgen
	LABELS startup uuid)

K3D_TEST(document.stress.high_node_count
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/document.stress.high_node_count.py
	LABELS document)

K3D_TEST(document.stress.all_node_types
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/document.stress.all_node_types.py
	LABELS document)

#K3D_TEST(document.stress.read_all_properties PYTHON)

K3D_TEST(document.serialization.properties
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/document.serialization.properties.py
	LABELS document)

K3D_TEST(document.importer.bogus_input
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/document.importer.bogus_input.py
	LABELS document importer)

K3D_TEST(notifier.InotifyFileChangeNotifier
	K3D_PYTHON_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/notifier.InotifyFileChangeNotifier.py
	REQUIRES K3D_BUILD_INOTIFY_MODULE
	LABELS notifier InotifyFileChangeNotifier)

