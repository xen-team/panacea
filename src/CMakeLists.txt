file(GLOB_RECURSE PANACEA_HEADERS
		RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		"*.h" "*.hpp" "*.inl"
		)
file(GLOB_RECURSE PANACEA_SOURCES
		RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		"*.c" "*.cpp"
		)

add_executable(panacea ${PANACEA_HEADERS} ${PANACEA_SOURCES})
target_link_libraries(panacea PRIVATE xen cpr::cpr)