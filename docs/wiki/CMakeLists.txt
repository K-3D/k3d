PROJECT(wiki)

IF(NOT WIN32)
	SET(K3D_NUI_BINARY ${k3d_BINARY_DIR}/lib/k3d/uiplugins/k3d-nui.module)

	CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/make-wiki-docs.py ${CMAKE_CURRENT_BINARY_DIR}/make-wiki-docs.py)
	FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/wikitext)
	FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/wikitext/plugins/articles)
	FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/wikitext/plugins/categories)
	FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/wikitext/plugins/reference)

	ADD_CUSTOM_TARGET(docs-wiki
		COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext/plugins/articles
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext/plugins/categories
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wikitext/plugins/reference
		COMMAND ${k3d_BINARY_DIR}/bin/k3d --log-level=debug --color
		--plugins=${k3d_BINARY_DIR}/lib/k3d/plugins
		--share=${k3d_SOURCE_DIR}/share
		--script=${CMAKE_CURRENT_BINARY_DIR}/make-wiki-docs.py
		--ui=${K3D_NUI_BINARY}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)
ENDIF(NOT WIN32)

