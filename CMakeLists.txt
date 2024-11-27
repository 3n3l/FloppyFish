cmake_minimum_required(VERSION 3.30)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0017 NEW)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0022 NEW)

project("Floppy Fish" C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

# Required libraries
find_package(Qt6OpenGL 6.7.3 REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "1")

# Process resource files.
# qt6_add_resources (ResourceSources images.qrc shader.qrc)

# Optional libraries.
find_package(GTA QUIET)

# Utility library.
add_subdirectory(glbase)

# Project files.
add_executable(FloppyFish
    main.cpp
    ${ResourceSources}
    # image/image.cpp
    # image/image.h
    # gui/mainwindow.ui
    gui/mainwindow.cpp
    gui/mainwindow.hpp
    gui/config.cpp
    gui/config.h

    # shader/skybox.vs.glsl
    # shader/skybox.fs.glsl
)

include_directories(${CMAKE_SOURCE_DIR}/glbase ${OPENGL_INCLUDE_DIR})

# Build with correct opengl lib.
if(WIN32 OR CYGWIN)
        target_link_libraries(FloppyFish opengl32 libglbase Qt::OpenGL ${OPENGL_gl_LIBRARY})
elseif(APPLE)
        target_link_libraries(FloppyFish libglbase Qt::OpenGL ${OPENGL_gl_LIBRARY})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenGL")
else()
        target_link_libraries(FloppyFish GL libglbase Qt::OpenGL ${OPENGL_gl_LIBRARY})
endif()

if(GTA_FOUND)
        add_definitions(-DHAVE_GTA)
        include_directories(${GTA_INCLUDE_DIR})
        target_link_libraries(FloppyFish ${GTA_LIBRARIES})
endif()

# copy images
# add_custom_command(TARGET FloppyFish POST_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy_directory
#                        ${CMAKE_SOURCE_DIR}/images $<TARGET_FILE_DIR:FloppyFish>/images)

install(TARGETS FloppyFish RUNTIME DESTINATION bin)
