cmake_minimum_required(VERSION 3.30)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
cmake_policy(SET CMP0017 NEW)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0022 NEW)

project("Floppy Fish" C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

# Required libraries.
find_package(OpenGL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL)

# Create a compile_commands.json.
set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "1")

# Optional libraries.
find_package(GTA QUIET)

# Project files.
add_executable(
  FloppyFish
  # Project entry.
  src/main.cpp
  # Source files.
  src/objects/background.cpp
  src/objects/background.h
  src/gui/mainwindow.cpp
  src/gui/mainwindow.h
  src/config/config.cpp
  src/config/config.h
  src/utils/utils.cpp
  src/utils/utils.h
  src/utils/image.cpp
  src/utils/image.h
  # Shaders.
  src/shaders/background.vs.glsl
  src/shaders/background.fs.glsl)

# Libraries.
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/assimp EXCLUDE_FROM_ALL)
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)

# Build with correct OpenGL library.
if(WIN32 OR CYGWIN)
  target_link_libraries(FloppyFish opengl32 Qt::Core Qt::Widgets Qt::OpenGL glm)
elseif(APPLE)
  target_link_libraries(FloppyFish Qt::Core Qt::Widgets Qt::OpenGL glm)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework OpenGL")
else()
  target_link_libraries(FloppyFish GL Qt::Core Qt::Widgets Qt::OpenGL glm)
endif()

if(GTA_FOUND)
  add_definitions(-DHAVE_GTA)
  include_directories(${GTA_INCLUDE_DIR})
  target_link_libraries(FloppyFish ${GTA_LIBRARIES})
endif()

install(TARGETS FloppyFish RUNTIME DESTINATION bin)
