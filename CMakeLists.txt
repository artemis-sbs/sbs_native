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

# include(FetchContent)
# FetchContent_Declare(
# 	glm
# 	GIT_REPOSITORY	https://github.com/g-truc/glm.git
# 	GIT_TAG 	bf71a834948186f4097caa076cd2663c69a10e1e #refs/tags/1.0.1
# )

# FetchContent_MakeAvailable(glm)
#include(glm.cmake)

add_subdirectory(deps)
#include_directories(deps)

pybind11_add_module(sbs_native src/main.cpp src/cosmos/image.cpp "src/cosmos/colors.cpp")
target_include_directories(sbs_native PUBLIC "src/cosmos")
#pybind11_LIBRARIES(sbs_native PUBLIC FastNoise)
#add_library(sbs_native src/main.cpp)
target_link_libraries(sbs_native PRIVATE FastNoise glm::glm)
#target_compile_features(sbs_native PRIVATE cxx_std_20)

# Add source to this project's executable.
add_executable (test "test_main.cpp" "src/cosmos/point_scatter.cpp" "src/cosmos/image.cpp" "test_main.h" "src/cosmos/map_data.cpp" "src/cosmos/colors.cpp")
#set(raylib_VERBOSE 1)
target_link_libraries(${PROJECT_NAME} FastNoise glm::glm)
#target_compile_features(test PRIVATE cxx_std_20)



# add_executable (gui  "src/gui/main.cpp")
# target_include_directories(gui PUBLIC "src/cosmos")
# target_link_libraries(gui PRIVATE SDL2::SDL2-static imgui )
# if (MSVC)
#     target_compile_options(gui PRIVATE /W4)
# else()
#     target_compile_options(gui PRIVATE -Wall -Wextra -pedantic)
# endif()
#target_copy_webgpu_binaries(gui)

add_executable (gui  "src/gui/main_rl.cpp" "src/cosmos/point_scatter.cpp" "src/cosmos/image.cpp" "src/cosmos/map_data.cpp" "src/cosmos/colors.cpp")
target_include_directories(gui PUBLIC "src/cosmos")

target_link_options(gui PRIVATE -mwindows)
target_link_libraries(gui PRIVATE raylib rlimgui imgui FastNoise glm::glm)
if (MSVC)
    target_compile_options(gui PRIVATE /W4)
else()
    target_compile_options(gui PRIVATE -Wall -Wextra -pedantic)
endif()


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test PROPERTY CXX_STANDARD 20)
  set_property(TARGET gui PROPERTY CXX_STANDARD 20)
  set_property(TARGET sbs_native PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
