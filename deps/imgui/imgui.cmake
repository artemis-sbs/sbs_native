include(FetchContent)

if (NOT TARGET imgui) 
	set(IMGUI_VERSION 1.89.8)
	set(IMGUI_VERSION_git v1.89.8) #-docking
	find_package(imgui ${IMGUI_VERSION} QUIET) # QUIET or REQUIRED
	if (NOT imgui_FOUND) # If there's none, fetch and build raylib
        include(FetchContent)
        FetchContent_Declare(
            imgui
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/ocornut/imgui/archive/refs/tags/${IMGUI_VERSION_git}.tar.gz
        )
        FetchContent_GetProperties(imgui)
        if (NOT imgui_POPULATED) # Have we downloaded raylib yet?
            set(FETCHCONTENT_QUIET NO)
            FetchContent_Populate(imgui)
            #add_subdirectory(${imgui_SOURCE_DIR} ${imgui_BINARY_DIR})
        endif()
	endif()

endif()



add_library(imgui STATIC
    # Among the different backends available, we are interested in connecting
    # the GUI to GLFW andWebGPU:
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp

    # Bonus to add some C++ specific features (the core ImGUi is a C library)
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp

    # The core ImGui files
    ${imgui_SOURCE_DIR}/imconfig.h
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imstb_rectpack.h
    ${imgui_SOURCE_DIR}/imstb_textedit.h
    ${imgui_SOURCE_DIR}/imstb_truetype.h
)

target_include_directories(imgui 
    PUBLIC ${imgui_SOURCE_DIR}
    PUBLIC ${imgui_SOURCE_DIR}/beckends
    PUBLIC ${imgui_SOURCE_DIR}/include
    )
target_link_libraries(imgui PUBLIC SDL2::SDL2-static)
