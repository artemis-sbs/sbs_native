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
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std:c++20")

project ("test" LANGUAGES CXX C)

add_subdirectory(deps)

pybind11_add_module(sbs_native src/main.cpp src/image.cpp "src/colors.cpp")
#pybind11_LIBRARIES(sbs_native PUBLIC FastNoise)
#add_library(sbs_native src/main.cpp)
target_link_libraries(sbs_native PRIVATE FastNoise)
#target_compile_features(sbs_native PRIVATE cxx_std_20)

# Add source to this project's executable.
add_executable (test "test_main.cpp" "src/point_scatter.cpp" "src/image.cpp" "test_main.h" "src/map_data.cpp" "src/colors.cpp")
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} FastNoise)
#target_compile_features(test PRIVATE cxx_std_20)



add_executable (gui  "src/gui/main.cpp" )
target_link_libraries(gui PRIVATE SDL2::SDL2 imgui )
if (MSVC)
    target_compile_options(gui PRIVATE /W4)
else()
    target_compile_options(gui PRIVATE -Wall -Wextra -pedantic)
endif()
#target_copy_webgpu_binaries(gui)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
