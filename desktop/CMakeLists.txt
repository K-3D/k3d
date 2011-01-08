configure_file(k3d.desktop.in k3d.desktop)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/k3d.desktop DESTINATION share/applications)
