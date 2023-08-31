set(SDL_TEST OFF)

add_subdirectory(SDL2)
#add_subdirectory(glfw)
add_subdirectory(pybind11)
add_subdirectory(fastnoise2)
#add_subdirectory(webgpu")
add_subdirectory(imgui)


add_library(vendor INTERFACE)
target_link_libraries(vendor INTERFACE SDL2::SDL2 )


