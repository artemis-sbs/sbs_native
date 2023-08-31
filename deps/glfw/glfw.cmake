include(FetchContent)

if (NOT TARGET webgpu) 
	set(GLFW_VERSION 3.3.8)
	set(GLFW_VERSION_git 3.3.8)
	find_package(glfw ${GLFW_VERSION} QUIET) # QUIET or REQUIRED
	if (NOT glfw_FOUND) # If there's none, fetch and build raylib
	include(FetchContent)
	FetchContent_Declare(
		glfw
		DOWNLOAD_EXTRACT_TIMESTAMP OFF
		URL https://github.com/glfw/glfw/archive/refs/tags/${GLFW_VERSION_git}.tar.gz
	)
	FetchContent_GetProperties(glfw)
	if (NOT glfw_POPULATED) # Have we downloaded raylib yet?
		set(FETCHCONTENT_QUIET NO)
		FetchContent_Populate(glfw)
		set(GLFW_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE) # don't build the supplied examples
		set(GLFW_BUILD_TESTS  OFF CACHE BOOL "" FORCE) # don't build the supplied examples
		set(GLFW_BUILD_DOCS  OFF CACHE BOOL "" FORCE) # don't build the supplied examples
		add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
	endif()
	endif()

endif()

