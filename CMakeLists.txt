# CMakeList.txt : CMake project for dasher, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.12) # FetchContent is available in 3.11+
# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std:c++20")

project ("test")

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
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${fastnoise2_SOURCE_DIR} ${fastnoise2_BINARY_DIR})
  endif()
endif()


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
#    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
    #add_subdirectory(pybind11)
  endif()
endif()


pybind11_add_module(sbs_native src/main.cpp src/image.cpp "src/colors.cpp")
#pybind11_LIBRARIES(sbs_native PUBLIC FastNoise)
#add_library(sbs_native src/main.cpp)
target_link_libraries(sbs_native PRIVATE FastNoise)

# Add source to this project's executable.
add_executable (test "test_main.cpp" "src/point_scatter.cpp" "src/image.cpp" "test_main.h" "src/map_data.cpp" "src/colors.cpp")
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} FastNoise)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
