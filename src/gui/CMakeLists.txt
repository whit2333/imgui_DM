cmake_minimum_required (VERSION 3.5)


configure_file(
  "ImGuiDMConfig.h.in"
  "${PROJECT_BINARY_DIR}/ImGuiDMConfig.h"
  )

find_package(Threads REQUIRED)

# --------------------------------------------------------

add_executable(imgui_dm 
  src/main.cpp
  src/helpers.cpp
  src/ImGuiDM.cpp
  #../3rd_party/addons/imguivariouscontrols/imguivariouscontrols.cpp
  #../3rd_party/metrics_gui/source/metrics_gui.cpp
  #include/ImGuiDM.h
  )
target_link_libraries(imgui_dm 
  ImGui 
  epics 
  Threads::Threads
  ${EPICS_LIBRARIES}
  ${ROOT_LIBRARIES} 
  ${OPENGL_LIBRARIES}
  )
target_include_directories(imgui_dm
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC include
  PUBLIC ../3rd_party/imgui
  PUBLIC ../3rd_party/addons/imguivariouscontrols
  PUBLIC ${EPICS_INCLUDE_DIRS}
  )
target_compile_features(imgui_dm
    PUBLIC cxx_auto_type
    PUBLIC cxx_trailing_return_types
    PRIVATE cxx_variadic_templates
    PRIVATE cxx_std_17
    )

install(TARGETS imgui_dm EXPORT ImguiDMTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
  )
# --------------------------------------------------------
#
#add_executable(demo2
#  src/demo.cpp
#  #src/imgui-SFML.cpp
#  )
#target_link_libraries(demo2 
#  ImGui 
#  ${ROOT_LIBRARIES} 
#  ${OPENGL_LIBRARIES}
#  )
#target_include_directories(demo2
#  PUBLIC include
#  PUBLIC src/3rd_party/imgui
#  )


# ----------------------------------------------
# Tests

# test_line
set(test_name test_line)
add_executable(${test_name} tests/${test_name}.cpp)
target_include_directories(${test_name} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PUBLIC src/3rd_party/imgui
  )
target_link_libraries(${test_name} 
  PRIVATE ImGui  ${OPENGL_LIBRARIES} -lGLU -lGL )
target_link_libraries(${test_name}
  PRIVATE Catch)
add_test(NAME ${test_name} COMMAND ${test_name})

## test_line
#set(test_name hello_triangle_indexed)
#add_executable(${test_name} 
#  tests/${test_name}.cpp
#  src/glad.c
#  )
#target_include_directories(${test_name} PUBLIC
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
#  PUBLIC src/3rd_party/imgui
#  )
#target_link_libraries(${test_name} 
#  PRIVATE ImGui  ${SFML_LIBRARIES}  ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES} -lGLEW -lGLU -lGL )
#target_link_libraries(${test_name}
#  PRIVATE Catch)
#add_test(NAME ${test_name} COMMAND ${test_name})
