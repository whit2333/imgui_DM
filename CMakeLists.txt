cmake_minimum_required(VERSION 2.6)
project(IMGUIDM)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules" "${CMAKE_SOURCE_DIR}/cmake")

# --------------------------------------------------------------------------
#
set(IMGUIDM_VERSION_MAJOR 1)
set(IMGUIDM_VERSION_MINOR 0)
set(IMGUIDM_FONT_DIR ${CMAKE_INSTALL_PREFIX}/share/imgui_DM/fonts)


configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
include_directories("${PROJECT_BINARY_DIR}")

# --------------------------------------------------------------------------
# Unit tests
enable_testing()
add_subdirectory(src/3rd_party/catch)


# --------------------------------------------------------------------------
# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 1.6 REQUIRED system window graphics network audio)

# --------------------------------------------------------------------------
# OpenGL
find_package(OpenGL REQUIRED)

# --------------------------------------------------------------------------
# EPICS (https://github.com/mdavidsaver/cmake4epics)
find_package(EPICSHostArch)
#find_package(EPICSTools)
include(TestBigEndian)
test_big_endian(target_endian)
message(STATUS "Target big endian=${target_endian}")
message(STATUS "Best guess at EPICS OS class and target")

find_package(EPICSHostArch)

message(STATUS "EPICS_HOST_CLASS=${EPICS_HOST_CLASS}")
message(STATUS "EPICS_HOST_CLASSES=${EPICS_HOST_CLASSES}")
message(STATUS "EPICS_HOST_COMPILER=${EPICS_HOST_COMPILER}")
message(STATUS "EPICS_HOST_ARCHS=${EPICS_HOST_ARCHS}")

find_package(EPICSTargetArch)

message(STATUS "EPICS_TARGET_CLASS=${EPICS_TARGET_CLASS}")
message(STATUS "EPICS_TARGET_CLASSES=${EPICS_TARGET_CLASSES}")
message(STATUS "EPICS_TARGET_COMPILER=${EPICS_TARGET_COMPILER}")
message(STATUS "EPICS_TARGET_ARCHS=${EPICS_TARGET_ARCHS}")

message(STATUS "Find EPICS Base")

find_package(EPICS COMPONENTS 
  ca IOC database pvAccess pvData pvaClient pvDatabase pvAccessCA nt)

message(STATUS "EPICS_FOUND=${EPICS_FOUND}")
message(STATUS "EPICS_BASE_DIR=${EPICS_BASE_DIR}")
message(STATUS "EPICS_INCLUDE_DIRS=${EPICS_INCLUDE_DIRS}")
message(STATUS "EPICS_TARGET_ARCH=${EPICS_TARGET_ARCH}")
message(STATUS "EPICS_LIBRARIES=${EPICS_LIBRARIES}")
message(STATUS "EPICS_Com_LIBRARY=${EPICS_Com_LIBRARY}")
message(STATUS "EPICS_ca_LIBRARY=${EPICS_ca_LIBRARY}")
message(STATUS "EPICS_IOC_LIBRARIES=${EPICS_IOC_LIBRARIES}")
message(STATUS "EPICSBase_VERSION=${EPICSBase_VERSION}")
message(STATUS "EPICSBase_VERSION_STRING=${EPICSBase_VERSION_STRING}")
message(STATUS "EPICS_DEFINITIONS=${EPICS_DEFINITIONS}")


# Find Libraries
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED 
  COMPONENTS
  MathCore RIO Hist MathMore Graf
  Graf3d Gpad GX11 Gviz3d RGL Eve
  #GeomPainter X3d Gviz3d RGL
  #Eve EG Foam GenVector Geom GLEW Gui HistPainter MathCore Matrix MLP Net
  Physics Spectrum Thread Tree TreeViewer)
include(${ROOT_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
#add_definitions(${ROOT_CXX_FLAGS})

add_subdirectory(src/3rd_party)
add_subdirectory(src/epics)
add_subdirectory(src/gui)
#add_subdirectory(src/sfml_root)
#add_subdirectory(src/gl_test)

install(DIRECTORY share/fonts
  DESTINATION share/imgui_DM
  )


## CPack packaging
#include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
#set(CPACK_PACKAGE_VERSION_MAJOR "${IMGUIDM_VERSION_MAJOR}")
#set(CPACK_PACKAGE_VERSION_MINOR "${IMGUIDM_VERSION_MINOR}")
#include(CPack)

