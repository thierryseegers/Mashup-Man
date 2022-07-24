cmake_minimum_required(VERSION 3.0.0)
project(Mashup-Man VERSION 1.0.0)

if(GENERATOR_IS_MULTI_CONFIG)
    set(CMAKE_BUILD_TYPES "Debug;MinSizeRel;Release;RelWithDebInfo")
endif()

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

# Request Conan to generate *-config.cmake modules for the dependencies.

# Conan won't install Boost.Math on macOS because it claims apple-clang is not c++11-capable.
# For that platform, Boost.Graph should be installed via other means (i.e. brew).
set(BOOST_PACKAGE_VERSION 1.79.0)
if(NOT APPLE)
    set(CONAN_BOOST_PACKAGE boost/${BOOST_PACKAGE_VERSION})
    set(CONAN_BOOST_PACKAGE_OPTIONS boost:without_graph=False)
endif()

set(CONAN_INSTALL_FOLDER ${CMAKE_BINARY_DIR}/conan)

# The new and preferred method for invoking Conan.
conan_cmake_configure(
    GENERATORS
        cmake_find_package_multi
    OPTIONS
        ${CONAN_BOOST_PACKAGE_OPTIONS}
        sfml:network=False
    REQUIRES
        ${CONAN_BOOST_PACKAGE}
        magic_enum/0.7.2
        tomlplusplus/2.3.0
        sfml/2.5.1
        spdlog/1.10.0
        zlib/1.2.12
)

foreach(TYPE ${CMAKE_CONFIGURATION_TYPES};${CMAKE_BUILD_TYPE})
    conan_cmake_autodetect(
        settings
        BUILD_TYPE
            ${TYPE}
    )

    conan_cmake_install(
        BASIC_SETUP
            NO_OUTPUT_DIRS
        BUILD
            missing
        INSTALL_FOLDER
            ${CONAN_INSTALL_FOLDER}
        # OUTPUT_QUIET
        PATH_OR_REFERENCE
            ${CMAKE_BINARY_DIR}
        SETTINGS
            ${settings}
    )
endforeach()
set(CMAKE_PREFIX_PATH ${CONAN_INSTALL_FOLDER})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
if(APPLE)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0057 NEW)
endif()
find_package(Boost ${BOOST_PACKAGE_VERSION}
    CONFIG
    COMPONENTS graph
    REQUIRED
)
if(APPLE)
    cmake_policy(POP)
endif()

find_package(magic_enum 0.7.2
    CONFIG
    REQUIRED
)

find_package(SFML 2.5.1
    CONFIG
    COMPONENTS audio graphics system window
    REQUIRED
)

find_package(spdlog 1.9.2
    CONFIG
    REQUIRED
)

find_package(tomlplusplus 2.3.0
    CONFIG
    REQUIRED
)

add_executable(${PROJECT_NAME}
    3rdparty/SFML/Graphics/RoundedRectangleShape.cpp
    3rdparty/SFML/Graphics/RoundedRectangleShape.hpp
    src/action.h
    src/application.cpp
    src/application.h
    src/astar_route.cpp
    src/astar_route.h
    src/background.cpp
    src/background.h
    src/bindings.cpp
    src/bindings.h
    src/command.h
    src/configuration.cpp
    src/configuration.h
    src/direction.cpp
    src/direction.h
    src/entity/animation.cpp
    src/entity/animation.h
    src/entity/character.cpp
    src/entity/character.h
    src/entity/enemy.cpp
    src/entity/enemy.h
    src/entity/entities.h
    src/entity/entity.cpp
    src/entity/entity.h
    src/entity/hero.cpp
    src/entity/hero.h
    src/entity/picker.cpp
    src/entity/picker.h
    src/entity/pipe.cpp
    src/entity/pipe.h
    src/entity/power_up.cpp
    src/entity/power_up.h
    src/entity/projectile.cpp
    src/entity/projectile.h
    src/entity/super_mario/enemy.cpp
    src/entity/super_mario/enemy.h
    src/entity/super_mario/entities.h
    src/entity/super_mario/hero.cpp
    src/entity/super_mario/hero.h
    src/entity/super_mario/picker.cpp
    src/entity/super_mario/picker.h
    src/entity/super_mario/power_up.cpp
    src/entity/super_mario/power_up.h
    src/entity/super_mario/projectile.cpp
    src/entity/super_mario/projectile.h
    src/level.cpp
    src/level.h
    src/lifeboard.cpp
    src/lifeboard.h
    src/main.cpp
    src/maze.cpp
    src/maze.h
    src/player.cpp
    src/player.h
    src/resources.cpp
    src/resources.h
    src/scene.cpp
    src/scene.h
    src/scoreboard.cpp
    src/scoreboard.h
    src/sound.cpp
    src/sound.h
    src/sprite.cpp
    src/sprite.h
    src/state/character_select.cpp
    src/state/character_select.h
    src/state/game_over.cpp
    src/state/game_over.h
    src/state/game.cpp
    src/state/game.h
    src/state/id.h
    src/state/level_complete.cpp
    src/state/level_complete.h
    src/state/pause.cpp
    src/state/pause.h
    src/state/stack.cpp
    src/state/stack.h
    src/state/state.cpp
    src/state/state.h
    src/state/states.h
    src/state/title.cpp
    src/state/title.h
    src/tomlpp.h
    src/utility.cpp
    src/utility.h
    src/world.cpp
    src/world.h
)

if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            -pedantic
            -Wall
            -Werror
            -Wextra
    )
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            /MP
    )

    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            _USE_MATH_DEFINES
            /W4
            /WX
            NOMINMAX
    )
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/3rdparty
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Boost::graph
        magic_enum::magic_enum
        SFML::SFML
        spdlog::spdlog
        tomlplusplus::tomlplusplus
)

if(APPLE)
    # Required by SFML.
    target_link_options(${PROJECT_NAME}
        PRIVATE
            -ObjC
    )

    # Indirectly required by Boost.Graph (Graph requires Regex which requires ICU).
    target_link_directories(${PROJECT_NAME}
        PRIVATE
            /usr/local/opt/icu4c/lib
    )
endif()

set_property(
    TARGET ${PROJECT_NAME}
    PROPERTY CXX_STANDARD 20
)

get_target_property(PROJECT_SOURCES ${PROJECT_NAME} SOURCES)

source_group(TREE ${CMAKE_CURRENT_LIST_DIR}
    FILES ${PROJECT_SOURCES}
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
