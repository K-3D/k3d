# Generate an application bundle (new-style)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/package-binary-bundle.cpack.in ${CMAKE_CURRENT_BINARY_DIR}/package-binary-bundle.cpack)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist @ONLY)
ADD_CUSTOM_TARGET(package-binary-bundle
	${CMAKE_CPACK_COMMAND}
	--config ${CMAKE_CURRENT_BINARY_DIR}/package-binary-bundle.cpack
	)

# Upload the binary bundle to SourceForge for use with the FRS
ADD_CUSTOM_TARGET(upload-binary-bundle
	rsync -avP -e ssh ${CMAKE_CURRENT_BINARY_DIR}/k3d-setup-${K3D_VERSION}-${CMAKE_SYSTEM_PROCESSOR}.dmg ${K3D_SOURCEFORGE_USERNAME}@frs.sourceforge.net:uploads/
	)

