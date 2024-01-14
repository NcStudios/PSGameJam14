add_executable(${GAME}
        GameMain.cpp
        Assets.cpp
        Character.cpp
        Core.cpp
        Event.cpp
        FollowCamera.cpp
        GameplayOrchestrator.cpp
        MainScene.cpp
        Tree.cpp
        UI.cpp
)

target_compile_options(${GAME}
    PRIVATE
        ${GAME_COMPILER_FLAGS}
)

target_compile_definitions(${GAME}
    PRIVATE
        ${NC_COMPILE_DEFINITIONS}
)

if(${GAME_PROD_BUILD})
    target_link_libraries(${GAME}
        PRIVATE
            NcEngine
    )
else()
    target_link_libraries(${GAME}
        PRIVATE
            NcEngine-dev
    )
endif()

install(TARGETS     ${GAME}
        DESTINATION "game"
)

install(DIRECTORY   "${PROJECT_SOURCE_DIR}/assets/nca"
        DESTINATION "game/assets"
)

install(FILES       "${PROJECT_SOURCE_DIR}/config.ini"
        DESTINATION "game"
)

install(FILES       ${PROJECT_BINARY_DIR}/_deps/nc-convert-src/nc-convert.exe
        DESTINATION bin
)

install(DIRECTORY   "${PROJECT_BINARY_DIR}/_deps/ncengine-src/resources/nca"
        DESTINATION "game/assets"
)

install(DIRECTORY      "${PROJECT_BINARY_DIR}/_deps/ncengine-src/resources/shaders/compiled/"
        DESTINATION    "game/assets/shaders"
        FILES_MATCHING REGEX ".*\.(spv)"
)

install(DIRECTORY DESTINATION "game/scenes")
