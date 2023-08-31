
set(PYBIND11_VERSION 2.11.1)
set(PYBIND11_VERSION_git v2.11.1)
#find_package(pybind11 ${PYBIND11_VERSION} QUIET) # QUIET or REQUIRED
find_package(pybind11 QUIET) # QUIET or REQUIRED
if (NOT pybind11_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    pybind11
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/pybind/pybind11/archive/refs/tags/${PYBIND11_VERSION_git}.tar.gz
  )
  FetchContent_GetProperties(pybind11)
  if (NOT pybind11_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(pybind11)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
    #add_subdirectory(pybind11)
  endif()
endif()
