PROJECT(epydoc)

SET(K3D_NUI_BINARY ${k3d_BINARY_DIR}/lib/k3d/uiplugins/k3d-nui.module)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/make-epydoc-docs.py ${CMAKE_CURRENT_BINARY_DIR}/make-epydoc-docs.py)

ADD_CUSTOM_TARGET(docs-epydoc
	COMMAND ${k3d_BINARY_DIR}/bin/k3d --log-level=debug --color
	--plugins=${k3d_BINARY_DIR}/lib/k3d/plugins
	--share=${k3d_SOURCE_DIR}/share
	--script=${CMAKE_CURRENT_BINARY_DIR}/make-epydoc-docs.py
	--ui=${K3D_NUI_BINARY}
	)

