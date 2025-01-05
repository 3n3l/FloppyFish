cmake_minimum_required(VERSION 3.30)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0017 NEW)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0022 NEW)

project("Floppy Fish" C CXX)

set(CMAKE_CXX_STANDARD
    23
    CACHE STRING "" FORCE)

# Required libraries.
find_package(OpenGL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL)

# Create a compile_commands.json.
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "1")

# Optional libraries.
find_package(GTA QUIET)

# Add the assets.qrc file.
set(ASSETS assets.qrc)

# Add the shaders.qrc file.
set(SHADERS shaders.qrc)

# Project files.
add_executable(
    FloppyFish
    # Project entry.
    src/main.cpp
    # Source files.
    src/drawables/obstacles/obstacle.cpp
    src/drawables/obstacles/obstacle.h
    src/drawables/obstacles/part.cpp
    src/drawables/obstacles/part.h
    src/drawables/background.cpp
    src/drawables/background.h
    src/drawables/drawable.cpp
    src/drawables/drawable.h
    src/drawables/fish.cpp
    src/drawables/fish.h
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    src/config/config.cpp
    src/config/config.h
    src/utils/utils.cpp
    src/utils/utils.h
    src/objects/floppyMesh.cpp
    src/objects/floppyMesh.h
    src/utils/image.cpp
    src/utils/image.h
    # Shaders.
    ${SHADERS}
    # Assets.
    ${ASSETS}
    # Mini library.
    lib/tinyobj/tiny_obj_loader.h
    lib/tinyobj/tiny_obj_loader.cc
)

# Libraries.
set(GLM_BUILD_LIBRARY
    OFF
    CACHE BOOL "" FORCE) # Header only to avoid linking errors
add_subdirectory(lib/glm)

# Build with correct OpenGL library.
if (WIN32 OR CYGWIN)
  target_link_libraries(FloppyFish Qt::Core Qt::Widgets Qt::OpenGL glm::glm)
elseif (APPLE)
    target_link_libraries(FloppyFish Qt::Core Qt::Widgets Qt::OpenGL glm)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenGL")
else ()
    target_link_libraries(FloppyFish GL Qt::Core Qt::Widgets Qt::OpenGL glm)
endif ()

# set root directory in visual studio
set_property(TARGET FloppyFish PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
                                        "${CMAKE_SOURCE_DIR}")

if (GTA_FOUND)
    add_definitions(-DHAVE_GTA)
    include_directories(${GTA_INCLUDE_DIR})
    target_link_libraries(FloppyFish ${GTA_LIBRARIES})
endif ()

install(TARGETS FloppyFish RUNTIME DESTINATION bin)
