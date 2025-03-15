cmake_minimum_required(VERSION 3.10)

project(dozy_tiler C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-g -Wall -Wextra -D_DEFAULT_SOURCE)

set(SRCS 
    main.c 
    editor.c 
    input.c 
    tilemap.c 
    asset_loader.c 
    texture_cache.c 
    utils.c 
    ../../shared/logger.c 
    ../../shared/sprite.c
)

add_executable(${PROJECT_NAME} ${SRCS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(${PROJECT_NAME} SDL2 SDL2_image m)

# Directory to copy assets from
set(ASSETS_DIR ../../assets/Tilesets)

# After building the target, copy assets into bin/assets/tilesets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/tilesets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSETS_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets/tilesets
)

