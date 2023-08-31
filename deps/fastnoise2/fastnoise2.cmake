set(FASTNOISE2_VERSION 0.10.0)
set(FASTNOISE2_VERSION_git v0.10.0-alpha)
find_package(fastnoise2 ${FASTNOISE2_VERSION} QUIET) # QUIET or REQUIRED
if (NOT fastnoise2_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    fastnoise2
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/Auburn/FastNoise2/archive/refs/tags/${FASTNOISE2_VERSION_git}.tar.gz
  )
  FetchContent_GetProperties(fastnoise2)
  if (NOT fastnoise2_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(fastnoise2)
    set(BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${fastnoise2_SOURCE_DIR} ${fastnoise2_BINARY_DIR})
  endif()
endif()


