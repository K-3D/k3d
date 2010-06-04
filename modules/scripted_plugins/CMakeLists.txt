K3D_BUILD_MODULE(k3d-scripted-plugins)
TARGET_LINK_LIBRARIES(k3d-scripted-plugins
	${Boost_REGEX_LIBRARY}
	)
# We explicitly do-not create a module proxy for this module,
# since it produces a dynamic list of plugins at runtime

