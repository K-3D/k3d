PROJECT(web)

FIND_PROGRAM(MAKO_COMMAND
	NAMES mako-render mako-render-2.6
	)

SET(web_OUTPUT_DIR "${web_BINARY_DIR}/site")

SET(developer_OUTPUT_DIR "${web_OUTPUT_DIR}/developer")
SET(planet_OUTPUT_DIR "${web_OUTPUT_DIR}/planet")
SET(tracker_OUTPUT_DIR "${web_OUTPUT_DIR}/developer/tracker")
SET(www_OUTPUT_DIR "${web_OUTPUT_DIR}/www")

ADD_CUSTOM_TARGET(www-html
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${web_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${www_OUTPUT_DIR}/resources"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${www_OUTPUT_DIR}/resources/banners"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.css" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.png" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/logo.png" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/maintenance.html" "${www_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/resources/banner" "${www_OUTPUT_DIR}/resources"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${www_OUTPUT_DIR}/resources" chmod a+x banner
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/resources/banners/moon.jpeg" "${www_OUTPUT_DIR}/resources/banners/moon.jpeg"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/resources/banners/submarine.jpeg" "${www_OUTPUT_DIR}/resources/banners/submarine.jpeg"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/www/resources/banners/town.jpeg" "${www_OUTPUT_DIR}/resources/banners/town.jpeg"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/www" ${MAKO_COMMAND} "donations.html" > "${www_OUTPUT_DIR}/donations.html"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/www" ${MAKO_COMMAND} "downloads.html" > "${www_OUTPUT_DIR}/downloads.html"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/www" ${MAKO_COMMAND} "index.html" > "${www_OUTPUT_DIR}/index.html"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/www" ${MAKO_COMMAND} "video.html" > "${www_OUTPUT_DIR}/video.html"
	)

ADD_CUSTOM_TARGET(planet-html
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${web_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.css" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.png" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/planet/feed-icon-10x10.png" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/planet/logo.png" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/planet/planet.css" "${planet_OUTPUT_DIR}"
	COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/planet/planet" --templates "${CMAKE_CURRENT_SOURCE_DIR}/planet" --output "${planet_OUTPUT_DIR}"
	)

ADD_CUSTOM_TARGET(developer-html
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${web_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.css" "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/k3d.png" "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/developer/developer.css" "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/developer/logo.png" "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/developer" ${MAKO_COMMAND} "dashboard.html" > "${developer_OUTPUT_DIR}/dashboard.html"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/developer" ${MAKO_COMMAND} "dashboard-index.html" > "${developer_OUTPUT_DIR}/dashboard-index.html"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/developer" ${MAKO_COMMAND} "index.html" > "${developer_OUTPUT_DIR}/index.html"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/developer" ${MAKO_COMMAND} "documentation.html" > "${developer_OUTPUT_DIR}/documentation.html"
	)

ADD_CUSTOM_TARGET(tracker-html
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${web_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${developer_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E make_directory "${tracker_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/developer/tracker/tracker.css" "${tracker_OUTPUT_DIR}"
	COMMAND "${CMAKE_COMMAND}" -E chdir "${CMAKE_CURRENT_SOURCE_DIR}/developer/tracker" "${CMAKE_CURRENT_SOURCE_DIR}/developer/tracker/tracker" --project "${ditz_SOURCE_DIR}" --templates "${CMAKE_CURRENT_SOURCE_DIR}/developer/tracker" --output "${tracker_OUTPUT_DIR}"
	)

