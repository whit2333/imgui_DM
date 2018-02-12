cmake_minimum_required (VERSION 3.5)

# --------------------------------------------------------
# Because epics' build system is really stupid
add_library(epics SHARED
  src/PVGetList.cpp
  src/PassiveCAGetter.cpp
  src/PVMonitor.cpp
  )
#  include/PVGetList.h
#  include/PassiveCAGetter.h
#  include/PVMonitor.h

target_include_directories(epics
  PUBLIC ${EPICS_INCLUDE_DIRS}
  PRIVATE ${EPICS_INCLUDE_DIRS}
  INTERFACE ${EPICS_INCLUDE_DIRS}
)
target_compile_definitions(epics
  PUBLIC ${EPICS_DEFINITIONS}
  PRIVATE ${EPICS_DEFINITIONS}
  INTERFACE ${EPICS_DEFINITIONS}
)
target_link_libraries(epics
  ${EPICS_LIBRARIES}
)
set_target_properties(epics
  PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD_REQUIRED 17
)
target_compile_features(epics
    PUBLIC cxx_auto_type
    PUBLIC cxx_trailing_return_types
    PRIVATE cxx_variadic_templates
    PRIVATE cxx_std_17
    )
target_include_directories(epics
  PUBLIC include
  #PUBLIC imgui/gl3w
  #PUBLIC imgui/dock
  #PUBLIC imgui/ImWindow
  #PUBLIC imgui/ImWindowGLFW
  )
target_include_directories(epics PUBLIC
  $<BUILD_INTERFACE:${epics_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${epics_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)


# ----------------------------------------------
# Tests

# test_line
set(test_name test_PVGetList)
add_executable(${test_name} tests/${test_name}.cxx)
target_include_directories(${test_name} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PUBLIC src/3rd_party/imgui
  PUBLIC ${EPICS_INCLUDE_DIRS}
  )
target_link_libraries(${test_name} 
  PRIVATE epics  
  PRIVATE ImGui  
  ${EPICS_LIBRARIES}
  ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES} -lGLEW -lGLU -lGL
  )

target_link_libraries(${test_name}
  PRIVATE Catch)
add_test(NAME ${test_name} COMMAND ${test_name})


