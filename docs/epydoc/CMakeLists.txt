PROJECT(epydoc)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/make-epydoc-docs.py ${CMAKE_CURRENT_BINARY_DIR}/make-epydoc-docs.py)

ADD_CUSTOM_TARGET(docs-epydoc
	COMMAND ${k3d_BINARY_DIR}/bin/k3d --log-level=debug --color
	--script=${CMAKE_CURRENT_BINARY_DIR}/make-epydoc-docs.py
	--ui=nui
	)

