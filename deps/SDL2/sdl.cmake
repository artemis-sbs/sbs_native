#set(SDL_VERSION 2.82.1)
#set(SDL_VERSION_git release-2.82.2)
find_package(SDL2 ${SDL2_VERSION} QUIET) # QUIET or REQUIRED
if (NOT SDL2_FOUND) # If there's none, fetch and build raylib
  include(FetchContent)
  FetchContent_Declare(
    SDL2
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.28.2
    GIT_SHALLOW TRUE

  )
  FetchContent_MakeAvailable(SDL2)
endif()



#https://github.com/libsdl-org/sdl/archive/refs/tags/2.82.2.tar.gz