PROJECT(po)

IF(K3D_ENABLE_NLS)
	FIND_PROGRAM(K3D_INTLTOOL_UPDATE intltool-update)
	MARK_AS_ADVANCED(K3D_INTLTOOL_UPDATE)

	FIND_PROGRAM(K3D_XGETTEXT xgettext)
	MARK_AS_ADVANCED(K3D_XGETTEXT)

	FIND_PROGRAM(K3D_MSGMERGE msgmerge)
	MARK_AS_ADVANCED(K3D_MSGMERGE)

	FIND_PROGRAM(K3D_MSGFMT msgfmt)
	MARK_AS_ADVANCED(K3D_MSGFMT)

	SET(K3D_CATALOGS
		chef
		)

	SET(K3D_CATALOG_TARGETS
		)

	ADD_CUSTOM_TARGET(nls-update-sources
		${K3D_INTLTOOL_UPDATE} -m
		WORKING_DIRECTORY ${po_SOURCE_DIR}
		)

	ADD_CUSTOM_COMMAND(OUTPUT ${po_BINARY_DIR}/k3d.pot
		COMMAND ${K3D_XGETTEXT} --files-from ${po_SOURCE_DIR}/POTFILES.in --keyword=_ -o ${po_BINARY_DIR}/k3d.pot --copyright-holder="Timothy M. Shead" --msgid-bugs-address="k3d-development@lists.sourceforge.net" --no-wrap --no-location
		DEPENDS POTFILES.in
		WORKING_DIRECTORY ${k3d_SOURCE_DIR}
		)

	FOREACH(catalog ${K3D_CATALOGS})
		ADD_CUSTOM_TARGET(nls-update-${catalog}.po
			${K3D_MSGMERGE} ${po_SOURCE_DIR}/${catalog}.po ${po_BINARY_DIR}/k3d.pot -o ${catalog}.po --no-wrap --sort-output
			DEPENDS ${po_SOURCE_DIR}/${catalog}.po
			WORKING_DIRECTORY ${po_SOURCE_DIR}
			)

		FILE(MAKE_DIRECTORY "${po_BINARY_DIR}/${catalog}/LC_MESSAGES")

		ADD_CUSTOM_COMMAND(OUTPUT ${po_BINARY_DIR}/${catalog}/LC_MESSAGES/k3d.mo
			COMMAND ${K3D_MSGFMT} -c -o ${po_BINARY_DIR}/${catalog}/LC_MESSAGES/k3d.mo ${po_SOURCE_DIR}/${catalog}.po
			DEPENDS ${po_SOURCE_DIR}/${catalog}.po
			)

		INSTALL(FILES ${po_BINARY_DIR}/${catalog}/LC_MESSAGES/k3d.mo
			DESTINATION share/k3d/locale/${catalog}/LC_MESSAGES
			)

		SET(K3D_CATALOG_TARGETS ${K3D_CATALOG_TARGETS} ${po_BINARY_DIR}/${catalog}/LC_MESSAGES/k3d.mo)
	ENDFOREACH(catalog)

	ADD_CUSTOM_TARGET(nls ALL
		DEPENDS ${po_BINARY_DIR}/k3d.pot ${K3D_CATALOG_TARGETS}
		)

ENDIF(K3D_ENABLE_NLS)

