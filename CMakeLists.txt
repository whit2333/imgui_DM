cmake_minimum_required(VERSION 2.6)
project(IMGUIDM)

# --------------------------------------------------------------------------
# Unit tests
enable_testing()
add_subdirectory(src/3rd_party/catch)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
# Set version information in a config.h file
set(IMGUIDM_VERSION_MAJOR 1)
set(IMGUIDM_VERSION_MINOR 0)
configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
include_directories("${PROJECT_BINARY_DIR}")

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 1.6 REQUIRED system window graphics network audio)

find_package(OpenGL REQUIRED)

# Find Libraries
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED 
  COMPONENTS
  #MathCore RIO Hist MathMore Graf Graf3d
  Gpad GX11 
  #GeomPainter X3d Gviz3d RGL
  #Eve EG Foam GenVector Geom GLEW Gui HistPainter MathCore Matrix MLP Net
  Physics Spectrum Thread Tree TreeViewer)
include(${ROOT_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
#add_definitions(${ROOT_CXX_FLAGS})

add_subdirectory(src/3rd_party)
add_subdirectory(src/sfml_root)
add_subdirectory(src/gui)


## CPack packaging
#include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
#set(CPACK_PACKAGE_VERSION_MAJOR "${IMGUIDM_VERSION_MAJOR}")
#set(CPACK_PACKAGE_VERSION_MINOR "${IMGUIDM_VERSION_MINOR}")
#include(CPack)

