cmake_minimum_required(VERSION 3.20)
project("Game"
        VERSION   "1.0.0"
        LANGUAGES CXX C
)

option(GAME_PROD_BUILD "Build and link against NcEngine prod" OFF)

set(GAME game)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_DEBUG_POSTFIX d)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(GAME_COMPILER_FLAGS -Werror
                            -Wfatal-errors
                            -Wall
                            -Wextra
                            -Wpedantic
                            -Wshadow
                            -Wconversion
                            -Wno-cast-function-type
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(GAME_COMPILER_FLAGS /W4
                            /WX
                            /MP
    )
endif()

include(FetchContent)
set(NC_INSTALL_ENABLED OFF CACHE BOOL "" FORCE)
FetchContent_Declare(NcEngine
                     GIT_REPOSITORY https://github.com/NcStudios/NcEngine.git
                     GIT_TAG        origin/dev/game-jam
                     GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(NcEngine)

add_subdirectory(source)
