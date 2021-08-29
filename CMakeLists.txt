cmake_minimum_required(VERSION 3.0.0)
project(SuperPacBros VERSION 1.0.0)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Get Conan.
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                  "${CMAKE_BINARY_DIR}/conan.cmake"
        TLS_VERIFY ON)
endif()
include(
    ${CMAKE_BINARY_DIR}/conan.cmake
)

# Request Conan to install Find*.cmake modules for the dependencies.
set(CONAN_INSTALL_FOLDER ${CMAKE_BINARY_DIR}/conan)
conan_cmake_run(REQUIRES
                    magic_enum/0.7.2
                    tomlplusplus/2.3.0
                    sfml/2.5.1@bincrafters/stable
                    spdlog/1.8.2
                BASIC_SETUP
                BUILD
                    missing
                GENERATORS
                    cmake_find_package
                INSTALL_FOLDER
                    ${CONAN_INSTALL_FOLDER}
                NO_OUTPUT_DIRS
)
list(APPEND CMAKE_MODULE_PATH ${CONAN_INSTALL_FOLDER})

find_package(magic_enum 0.7.2 MODULE
    REQUIRED
)

find_package(SFML 2.5.1 MODULE
    COMPONENTS audio graphics network system window
    REQUIRED
)
add_library(sfml::sfml ALIAS SFML::SFML)

find_package(spdlog 1.8.2 MODULE
    REQUIRED
)

find_package(tomlplusplus 2.3.0 MODULE
    REQUIRED
)

add_executable(${PROJECT_NAME}
    src/application.cpp
    src/application.h
    src/command.h
    src/configuration.cpp
    src/configuration.h
    src/entity/animation.h
    src/entity/brother.cpp
    src/entity/brother.h
    src/entity/character.cpp
    src/entity/character.h
    src/entity/enemy.cpp
    src/entity/enemy.h
    src/entity/entity.h
    src/entity/pickup.cpp
    src/entity/pickup.h
    src/level.cpp
    src/level.h
    src/main.cpp
    src/maze.cpp
    src/maze.h
    src/resources.cpp
    src/resources.h
    src/scene.cpp
    src/scene.h
    src/sound.cpp
    src/sound.h
    src/tomlpp.h
    src/utility.cpp
    src/utility.h
    src/state/game.cpp
    src/state/game.h
    src/state/id.h
    src/state/stack.cpp
    src/state/stack.h
    src/state/state.cpp
    src/state/state.h
    src/world.cpp
    src/world.h
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
        -Werror
        -pedantic
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        magic_enum::magic_enum
        sfml::sfml
        spdlog::spdlog
        tomlplusplus::tomlplusplus
)

set_property(
    TARGET ${PROJECT_NAME}
    PROPERTY CXX_STANDARD 20
)

add_custom_target(copy-assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)

add_custom_target(copy-default-configuration ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/config.toml ${CMAKE_BINARY_DIR}/config.toml
)

add_dependencies(${PROJECT_NAME}
    copy-assets
    copy-default-configuration
)
